#include "Tank.hpp"

#include <iostream>

#include "DataTables.hpp"

#include <SFML/Graphics/RenderTarget.hpp>

#include "Projectile.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "DataTables.hpp"
#include "Pickup.hpp"
#include "PickupType.hpp"


namespace
{
	const std::vector<TankData> Table = InitializeTankData();
}


Textures ToTextureID(TankType type)
{
	switch (type)
	{
	case TankType::kCamo:
		return Textures::kCamo;
	case TankType::kSand:
		return Textures::kSand;
	case TankType::kGreen:
		return Textures::kGreen;
	}
	return Textures::kCamo;
}

Tank::Tank(TankType type, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[static_cast<int>(type)].m_hitpoints)
	, m_type(type)
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture))
	, m_is_firing(false)
	, m_is_launching_missile(false)
	, m_fire_countdown(sf::Time::Zero)
	, m_is_marked_for_removal(false)
	, m_fire_rate(1)
	, m_spread_level(1)
	, m_missile_ammo(2)
	, m_health_display(nullptr)
	, m_missile_display(nullptr)
	, m_travelled_distance(0.f)
	, m_directions_index(0)
{
	// Reduce Tank sprite size
	m_sprite.setScale(0.5f, 0.5f);

	Utility::CentreOrigin(m_sprite);

	m_fire_command.category = static_cast<int>(Category::Type::kScene);
	m_fire_command.action = [this, &textures](SceneNode& node, sf::Time)
	{
		CreateBullets(node, textures);
	};

	m_missile_command.category = static_cast<int>(Category::Type::kScene);
	m_missile_command.action = [this, &textures](SceneNode& node, sf::Time)
	{
		CreateProjectile(node, ProjectileType::kMissile, 0.f, 0.5f, textures);
	};

	m_drop_pickup_command.category = static_cast<int>(Category::Type::kScene);
	m_drop_pickup_command.action = [this, &textures](SceneNode& node, sf::Time)
	{
		CreatePickup(node, textures);
	};

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	m_health_display = healthDisplay.get();
	AttachChild(std::move(healthDisplay));

	if (Tank::GetCategory() == static_cast<int>(Category::kPlayerTank))
	{
		std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
		missileDisplay->setPosition(0, 70);
		m_missile_display = missileDisplay.get();
		AttachChild(std::move(missileDisplay));
	}

	UpdateTexts();

}

void Tank::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
}

unsigned int Tank::GetCategory() const
{
	if (IsAllied())
	{
		return static_cast<int>(Category::kPlayerTank);
	}
	return static_cast<int>(Category::kEnemyTank);
}

void Tank::IncreaseFireRate()
{
	if (m_fire_rate < 10)
	{
		++m_fire_rate;
	}
}

void Tank::IncreaseSpread()
{
	if (m_spread_level < 3)
	{
		++m_spread_level;
	}
}

void Tank::CollectMissiles(unsigned int count)
{
	m_missile_ammo += count;
}

void Tank::UpdateTexts()
{
	m_health_display->SetString(std::to_string(GetHitPoints()) + "HP");
	m_health_display->setPosition(0.f, 50.f);
	m_health_display->setRotation(-getRotation());

	if (m_missile_display)
	{
		if (m_missile_ammo == 0)
		{
			m_missile_display->SetString("");
		}
		else
		{
			m_missile_display->SetString("M: " + std::to_string(m_missile_ammo));
		}
	}

}

void Tank::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{
	if (IsDestroyed())
	{
		CheckPickupDrop(commands);
		m_is_marked_for_removal = true;
		return;
	}
	//Check if bullets or missiles are fired
	CheckProjectileLaunch(dt, commands);
	// Update enemy movement pattern; apply velocity
	UpdateMovementPattern(dt);
	Entity::UpdateCurrent(dt, commands);
	UpdateTexts();
}

void Tank::UpdateMovementPattern(sf::Time dt)
{
	//Enemy AI
	const std::vector<Direction>& directions = Table[static_cast<int>(m_type)].m_directions;
	if (!directions.empty())
	{
		//Move along the current direction, change direction
		if (m_travelled_distance > directions[m_directions_index].m_distance)
		{
			m_directions_index = (m_directions_index + 1) % directions.size();
			m_travelled_distance = 0.f;
		}

		//Compute velocity from direction
		double radians = Utility::ToRadians(directions[m_directions_index].m_angle + 90.f);
		float vx = GetMaxSpeed() * std::cos(radians);
		float vy = GetMaxSpeed() * std::sin(radians);

		SetVelocity(vx, vy);
		m_travelled_distance += GetMaxSpeed() * dt.asSeconds();

	}


}

float Tank::GetMaxSpeed() const
{
	return Table[static_cast<int>(m_type)].m_speed;
}

float Tank::GetRotationSpeed() const
{
	return Table[static_cast<int>(m_type)].m_rotationSpeed;
}

void Tank::Fire()
{
	//Only ships with a non-zero fire interval fire
	if (Table[static_cast<int>(m_type)].m_fire_interval != sf::Time::Zero)
	{
		m_is_firing = true;
	}
}

void Tank::LaunchMissile()
{
	if (m_missile_ammo > 0)
	{
		m_is_launching_missile = true;
		--m_missile_ammo;
	}
}

void Tank::CheckProjectileLaunch(sf::Time dt, CommandQueue& commands)
{
	//Enemies try and fire as often as possible
	if (!IsAllied())
	{
		Fire();
	}

	//Rate the bullets - default to 2 times a second
	if (m_is_firing && m_fire_countdown <= sf::Time::Zero)
	{
		//Countdown expired, can fire again
		std::cout << "Pushing fire command" << std::endl;
		commands.Push(m_fire_command);
		m_fire_countdown += Table[static_cast<int>(m_type)].m_fire_interval / (m_fire_rate + 1.f);
		m_is_firing = false;
	}
	else if (m_fire_countdown > sf::Time::Zero)
	{
		//Wait, can't fire yet
		m_fire_countdown -= dt;
		m_is_firing = false;
	}
	//Missile launch
	if (m_is_launching_missile)
	{
		commands.Push(m_missile_command);
		m_is_launching_missile = false;
	}

}

bool Tank::IsAllied() const
{
	return m_type == TankType::kCamo;
}


//TODO Do enemies need a different offset as they are flying down the screen?
void Tank::CreateBullets(SceneNode& node, const TextureHolder& textures) const
{
	ProjectileType type = IsAllied() ? ProjectileType::kAlliedBullet : ProjectileType::kEnemyBullet;
	switch (m_spread_level)
	{
	case 1:
		CreateProjectile(node, type, 0.0f, 0.5f, textures);
		break;
	case 2:
		CreateProjectile(node, type, -0.5f, 0.5f, textures);
		CreateProjectile(node, type, 0.5f, 0.5f, textures);
		break;
	case 3:
		CreateProjectile(node, type, -0.5f, 0.5f, textures);
		CreateProjectile(node, type, 0.0f, 0.5f, textures);
		CreateProjectile(node, type, 0.5f, 0.5f, textures);
		break;

	}

}

void Tank::CreateProjectile(SceneNode& node, ProjectileType type, float x_offset, float y_offset,
	const TextureHolder& textures) const
{
	std::cout << "Creating projectile " << static_cast<int>(type) << std::endl;
	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f offset(x_offset * m_sprite.getGlobalBounds().width, y_offset * m_sprite.getGlobalBounds().height);
	sf::Vector2f velocity(0, projectile->GetMaxSpeed());

	float sign = IsAllied() ? -1.f : +1.f;
	projectile->setPosition(GetWorldPosition() + offset * sign);
	projectile->SetVelocity(velocity * sign);
	node.AttachChild(std::move(projectile));
}

sf::FloatRect Tank::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

bool Tank::IsMarkedForRemoval() const
{
	return m_is_marked_for_removal;
}

void Tank::CheckPickupDrop(CommandQueue& commands)
{
	if (!IsAllied() && Utility::RandomInt(3) == 0)
	{
		commands.Push(m_drop_pickup_command);
	}
}

void Tank::CreatePickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(GetWorldPosition());
	pickup->SetVelocity(0.f, 1.f);
	node.AttachChild(std::move(pickup));
}