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
#include "SoundNode.hpp"
#include "NetworkNode.hpp"


namespace
{
	const std::vector<TankData> Table = InitializeTankData();
}


Tank::Tank(TankType type, TankType cannonType, const TextureHolder& textures, const FontHolder& fonts)
	: Entity(Table[static_cast<int>(type)].m_hitpoints)
	, m_type(type)
	, m_cannon_type(cannonType)
	, m_sprite(textures.Get(Table[static_cast<int>(type)].m_texture))
	, m_cannon_sprite(textures.Get(Table[static_cast<int>(cannonType)].m_cannon_texture))
	, m_is_firing(false)
	, m_is_launching_missile(false)
	, m_fire_countdown(sf::Time::Zero)
	, m_move_sound_countdown(sf::Time::Zero)
	, m_move_cannon_sound_countdown(sf::Time::Zero)
	, m_is_marked_for_removal(false)
	, m_fire_rate(1)
	, m_extra_bullet_level(1)
	, m_tank_speed_level(1)
	, m_missile_ammo(2)
	, m_health_display(nullptr)
	, m_missile_display(nullptr)
	, m_travelled_distance(0.f)
	, m_directions_index(0)
	, m_cannon_rotation(0)
	, m_played_explosion_sound(false)
	, m_is_playing_move_sound(false)
	, m_is_playing_cannon_move_sound(false)
	, m_identifier(0)
	, m_is_pickups_spawned(false)
{
	// Reduce Tank sprite size
	m_sprite.setScale(0.5f, 0.5f);
	m_cannon_sprite.setScale(0.5f, 0.5f);

	//// Rotate depending on type
	//if (m_type == TankType::kCamo)
	//{
	//	this->setRotation(90);
	//}
	//else
	//{
	//	this->setRotation(-90);
	//}

	//SetVelocity(0, 0);
	          
	// Set Origin of Tank and Cannon
	Utility::CentreOrigin(m_sprite);

	sf::FloatRect cannonBounds = m_cannon_sprite.getLocalBounds();
	m_cannon_sprite.setOrigin(std::floor(cannonBounds.left + cannonBounds.width / 2.0f), std::floor(cannonBounds.top + cannonBounds.height / 1.43f));

	// Adjust cannon position on the Tank
	sf::Vector2f cannonOriginOffset = sf::Vector2f(1.0f, 2.0f);
	m_cannon_sprite.setPosition(cannonOriginOffset);

	// Create Nodes
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

	/*m_drop_pickup_command.category = static_cast<int>(Category::Type::kScene);
	m_drop_pickup_command.action = [this, &textures](SceneNode& node, sf::Time)
	{
		CreatePickup(node, textures);
	};*/

	std::unique_ptr<TextNode> healthDisplay(new TextNode(fonts, ""));
	m_health_display = healthDisplay.get();
	AttachChild(std::move(healthDisplay));

	std::unique_ptr<TextNode> missileDisplay(new TextNode(fonts, ""));
	missileDisplay->setPosition(0, 70);
	m_missile_display = missileDisplay.get();
	AttachChild(std::move(missileDisplay));

	UpdateTexts();

	
}

int Tank::GetMissileAmmo() const
{
	return m_missile_ammo;
}

void Tank::SetMissileAmmo(int ammo)
{
	m_missile_ammo = ammo;
}

void Tank::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(m_sprite, states);
	target.draw(m_cannon_sprite, states);
}

void Tank::DisablePickups()
{
	m_pickups_enabled = false;
}

unsigned int Tank::GetCategory() const
{
	if (IsPlayerTank())
	{
		if (m_type == TankType::kCamo)
		{
			return static_cast<int>(Category::kPlayerTank);
		}
		else
		{
			return static_cast<int>(Category::kPlayer2Tank);
		}
	}
	return static_cast<int>(Category::kEnemyTank);
}

void Tank::IncreaseFireRate()
{
	if (m_fire_rate < 3)
	{
		++m_fire_rate;
	}
}

void Tank::IncreaseBullets()
{
	if (m_extra_bullet_level < 3)
	{
		++m_extra_bullet_level;
	}
}

void Tank::IncreaseTankSpeed()
{
	if (m_tank_speed_level < 1.5f)
	{
		m_tank_speed_level += 0.5f;
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

void Tank::CheckProjectileLaunch(sf::Time dt, CommandQueue& commands)
{

	// Rate the bullets 
	if (m_is_firing && m_fire_countdown <= sf::Time::Zero)
	{
		// Countdown expired, can fire again
		commands.Push(m_fire_command);

		// Play shoot SFX
		PlayLocalSound(commands, SoundEffects::kNormalBulletFire);

		m_fire_countdown += Table[static_cast<int>(m_type)].m_fire_interval / (m_fire_rate + 1.f);
		m_is_firing = false;
	}
	else if (m_fire_countdown > sf::Time::Zero)
	{
		// Wait, can't fire yet
		m_fire_countdown -= dt;
		m_is_firing = false;
	}

	// Missile launch
	if (m_is_launching_missile)
	{
		PlayLocalSound(commands, SoundEffects::kLaunchGuidedMissile);
		commands.Push(m_missile_command);
		m_is_launching_missile = false;
	}
}

void Tank::UpdateCurrent(sf::Time dt, CommandQueue& commands)
{

	if (m_is_playing_move_sound && m_move_sound_countdown <= sf::Time::Zero)
	{
		// Countdown expired, can play Tank moving sound again

		PlayLocalSound(commands, SoundEffects::kTankMoving);

		m_move_sound_countdown += Table[static_cast<int>(m_type)].m_move_sound_interval;
		m_is_playing_move_sound = false;
	}
	else if (m_move_sound_countdown > sf::Time::Zero)
	{
		// Wait, can't play Tank moving sound yet
		m_move_sound_countdown -= dt;
		m_is_playing_move_sound = false;
	}

	if (m_is_playing_cannon_move_sound && m_move_cannon_sound_countdown <= sf::Time::Zero)
	{
		// Countdown expired, can play Tank cannon moving sound again

		PlayLocalSound(commands, SoundEffects::kTankCannonMoving);

		m_move_cannon_sound_countdown += Table[static_cast<int>(m_type)].m_move_cannon_sound_interval;
		m_is_playing_cannon_move_sound = false;
	}
	else if (m_move_cannon_sound_countdown > sf::Time::Zero)
	{
		// Wait, can't play Tank cannon moving sound yet
		m_move_cannon_sound_countdown -= dt;
		m_is_playing_cannon_move_sound = false;
	}

	UpdateTexts();
	
	if (IsDestroyed())
	{
		//CheckPickupDrop(commands);
		m_is_marked_for_removal = true;
		return;
	}

	//Check if bullets or missiles are fired
	CheckProjectileLaunch(dt, commands);

	// Update enemy movement pattern; apply velocity
	//UpdateMovementPattern(dt);

	Entity::UpdateCurrent(dt, commands);
	
}


int	Tank::GetIdentifier()
{
	return m_identifier;
}

void Tank::SetIdentifier(int identifier)
{
	m_identifier = identifier;
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
	return Table[static_cast<int>(m_type)].m_speed * m_tank_speed_level;
}

float Tank::GetRotationSpeed() const
{
	return Table[static_cast<int>(m_type)].m_rotation_speed;
}

float Tank::GetCannonRotationSpeed() const
{
	return Table[static_cast<int>(m_type)].m_cannon_rotation_speed;
}

float Tank::GetCannonRotationAngle() const
{
	return m_cannon_rotation;
}

TankType Tank::GetTankType() const
{
	return m_type;
}

void Tank::SetCannonAngle(float angle)
{
	m_cannon_rotation = angle;

	m_cannon_sprite.setRotation(angle);
}

void Tank::RotateCannon(float angle)
{
	m_cannon_rotation += angle;

	if (m_cannon_rotation > 359)
	{
		m_cannon_rotation = 0;
	}
	else if (m_cannon_rotation < 0)
	{
		m_cannon_rotation = 359;
	}

	m_cannon_sprite.rotate(angle);

}

void Tank::MoveSoundPlayInterval()
{
	// if the Tanks move sound interval is not zero check bool to true so you can play sound again
	if (Table[static_cast<int>(m_type)].m_move_sound_interval != sf::Time::Zero)
	{
		m_is_playing_move_sound = true;
	}
}

void Tank::CannonMoveSoundPlayInterval()
{
	// if the Tanks move sound interval is not zero check bool to true so you can play sound again
	if (Table[static_cast<int>(m_type)].m_move_cannon_sound_interval != sf::Time::Zero)
	{
		m_is_playing_cannon_move_sound = true;
	}
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

bool Tank::IsPlayerTank() const
{
	return m_type == TankType::kCamo || m_type == TankType::kSand;
}

void Tank::CreateBullets(SceneNode& node, const TextureHolder& textures) const
{
	ProjectileType type = IsPlayerTank() ? ProjectileType::kAlliedBullet : ProjectileType::kEnemyBullet;

	switch (m_extra_bullet_level)
	{
	case 1:
		CreateProjectile(node, type, 0.f, 0.4f, textures);
		break;
	case 2:
		CreateProjectile(node, type, 0.f, 0.4f, textures);
		CreateProjectile(node, type, 0.f, 0.6f, textures);
		break;
	case 3:
		CreateProjectile(node, type, 0.f, 0.4f, textures);
		CreateProjectile(node, type, 0.f, 0.6f, textures);
		CreateProjectile(node, type, 0.f, 0.8f, textures);
		break;
	}
}

void Tank::CreateProjectile(SceneNode& node, ProjectileType type, float x_offset, float y_offset,
	const TextureHolder& textures) const
{

	std::unique_ptr<Projectile> projectile(new Projectile(type, textures));
	sf::Vector2f offset(x_offset * m_sprite.getGlobalBounds().width, y_offset * m_sprite.getGlobalBounds().height);

	// Calculate x and y starting position of the projectile in terms of the angle of rotation of tank and cannon in degrees.
	float tankAngle = getRotation();
	float cannonAngle = GetCannonRotationAngle();
	float totalAngle = tankAngle + cannonAngle;
	float projectileAngle;

	// To get the combined angle of the Tank base and Tank cannon 
	// we must add the angles together if the sum is less than 180 
    // else if greater than 180 we add and then take away 360 because we want to always have an angle between 0 and 360
	if (totalAngle > 180)
	{
		projectileAngle = totalAngle - 360;
	}
	else
	{
		projectileAngle = totalAngle;
	}

	float projectileXPos = sinf(Utility::ToRadians(projectileAngle));
	float projectileYPos = -cosf(Utility::ToRadians(projectileAngle));

	// Join together in a sf::Vector2f
	sf::Vector2f projectilePos(projectileXPos, projectileYPos);
	
	// Added on 52.0f to the projectile position.y so the projectile will be outside their collision box and not damage itself.
	projectile->setPosition(GetWorldPosition() + projectilePos * (offset.x + offset.y + 52.0f));
	projectile->setRotation(projectileAngle);
	projectile->SetVelocity(projectilePos * projectile->GetMaxSpeed());
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
	if (!IsPlayerTank() && Utility::RandomInt(3) == 0)
	{
		commands.Push(m_drop_pickup_command);
	}
}

void Tank::CreatePickup(SceneNode& node, const TextureHolder& textures) const
{
	auto type = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup(new Pickup(type, textures));
	pickup->setPosition(GetWorldPosition());
	pickup->SetVelocity(0.f, 0.f);
	node.AttachChild(std::move(pickup));
}

void Tank::PlayLocalSound(CommandQueue& commands, SoundEffects effect)
{
	sf::Vector2f world_position = GetWorldPosition();

	Command command;
	command.category = Category::kSoundEffect;
	command.action = DerivedAction<SoundNode>(
		[effect, world_position](SoundNode& node, sf::Time)
		{
			node.PlaySound(effect, world_position);
		});

	commands.Push(command);
}