#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <limits>

#include "Pickup.hpp"
#include "Projectile.hpp"
#include "Utility.hpp"

World::World(sf::RenderWindow& window, FontHolder& font)
	: m_window(window)
	, m_camera(window.getDefaultView())
	, m_textures()
	, m_fonts(font)
	, m_scenegraph()
	, m_scene_layers()
	, m_world_bounds(0.f, 0.f, m_camera.getSize().x, m_camera.getSize().y)
	, m_spawn_position(m_camera.getSize().x / 2.f, m_world_bounds.height - m_camera.getSize().y / 2.f)
	, m_scrollspeed(0.f)
	, m_player_tank(nullptr)
	, m_player_tank_2(nullptr)
{
	LoadTextures();
	BuildScene();
	//std::cout << m_camera.getSize().x << m_camera.getSize().y << std::endl;
	m_camera.setCenter(m_spawn_position);
}

void World::Update(sf::Time dt)
{

	//Scroll the world
	m_camera.move(0, m_scrollspeed * dt.asSeconds());

	m_player_tank->SetVelocity(0.f, 0.f);
	m_player_tank_2->SetVelocity(0.f, 0.f);

	DestroyEntitiesOutsideView();
	GuideMissiles();

	//Forward commands to the scenegraph until the command queue is empty
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}
	AdaptPlayerVelocity();

	HandleCollisions();
	//Remove all destroyed entities
	m_scenegraph.RemoveWrecks();

	SpawnEnemies();

	//Apply movement
	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition();
}

void World::Draw()
{
	m_window.setView(m_camera);
	m_window.draw(m_scenegraph);
}

bool World::HasAlivePlayer1() const
{
	// If a Player 1 Tank is not destroyed, hence Player 1 is still alive
	return !m_player_tank->IsMarkedForRemoval();
}

bool World::HasAlivePlayer2() const
{
	// If a Player 2 Tank is not destroyed, hence Player 2 is still alive
	return !m_player_tank_2->IsMarkedForRemoval();
}

bool World::HasPlayerReachedEnd() const
{
	// If a Player Tank is not inside the screen / bounds.
	return !m_world_bounds.contains(m_player_tank->getPosition()) || !m_world_bounds.contains(m_player_tank_2->getPosition());
}

void World::LoadTextures()
{
	// Player 1 textures
	m_textures.Load(Textures::kCamo, "Media/Textures/Tank/Camo Tank/Camo Tank Base.png"); // Tank base
	m_textures.Load(Textures::kCannonCamo, "Media/Textures/Tank/Camo Tank/Camo Tank Turret.png"); // Tank cannon
	
	// Player 2 textures
	m_textures.Load(Textures::kSand, "Media/Textures/Tank/sand/Sand Tank Base.png"); // Tank base
	m_textures.Load(Textures::kCannonSand, "Media/Textures/Tank/sand/Sand Tank Turret.png"); // Tank cannon
	
	// Other Tank textures
	m_textures.Load(Textures::kGreen, "Media/Textures/Tank/green/Tank_B_Big_Green_2_128x194.png"); // base and cannon combined
	m_textures.Load(Textures::kBrown, "Media/Textures/Tank/brown/Tank_Big_Brown_128x194.png"); // base and cannon combined

	// Background textures
	m_textures.Load(Textures::kDesert, "Media/Textures/Desert.png"); // *** IMPORTANT *** ==> TEXTURE TO BE SWAPPED !!!

	// Projectile textures
	m_textures.Load(Textures::kBullet, "Media/Textures/Bullet.png");
	m_textures.Load(Textures::kMissile, "Media/Textures/Missile.png");
	m_textures.Load(Textures::kHealthRefill, "Media/Textures/HealthRefill.png");
	m_textures.Load(Textures::kMissileRefill, "Media/Textures/MissileRefill.png");
	m_textures.Load(Textures::kFireSpread, "Media/Textures/FireSpread.png");
	m_textures.Load(Textures::kFireRate, "Media/Textures/FireRate.png");
}

void World::BuildScene()
{
	//Initialize the different layers
	for (std::size_t i = 0; i < static_cast<int>(Layers::kLayerCount); ++i)
	{
		Category::Type category = (i == static_cast<int>(Layers::kAir)) ? Category::Type::kScene : Category::Type::kNone;
		SceneNode::Ptr layer(new SceneNode(category));
		m_scene_layers[i] = layer.get();
		m_scenegraph.AttachChild(std::move(layer));
	}

	//Prepare the background
	sf::Texture& texture = m_textures.Get(Textures::kDesert);
	sf::IntRect textureRect(m_world_bounds);
	//Tile the texture to cover our world
	texture.setRepeated(true);

	//Add the background sprite to our scene
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, textureRect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(background_sprite));

	//Add Player 1 Tank
	std::unique_ptr<Tank> leader(new Tank(TankType::kCamo, TankType::kCannonCamo, m_textures, m_fonts));
	m_player_tank = leader.get();
	m_player_tank->setPosition(m_spawn_position.x - 200.0f, m_spawn_position.y);
	m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(leader));

	//Add Player 2 Tank
	std::unique_ptr<Tank> leader2(new Tank(TankType::kSand, TankType::kCannonSand, m_textures, m_fonts));
	m_player_tank_2 = leader2.get();
	m_player_tank_2->setPosition(m_spawn_position.x + 200.0f, m_spawn_position.y);
	m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(leader2));


	// //Add two escorts
	// std::unique_ptr<Tank> leftEscort(new Tank(TankType::kSand, m_textures, m_fonts));
	// leftEscort->setPosition(-80.f, 50.f);
	// m_player_tank->AttachChild(std::move(leftEscort));
	//
	// std::unique_ptr<Tank> rightEscort(new Tank(TankType::kSand, m_textures, m_fonts));
	// rightEscort->setPosition(80.f, 50.f);
	// m_player_tank->AttachChild(std::move(rightEscort));

	AddEnemies();
}

CommandQueue& World::getCommandQueue()
{
	return m_command_queue;
}

void World::AdaptPlayerPosition()
{
	sf::FloatRect view_bounds = GetViewBounds();
	const float border_distance = 40.f;

	// Keep Player 1 Tank inside the screen / bounds
	sf::Vector2f player1Pos = m_player_tank->getPosition();
	player1Pos.x = std::max(player1Pos.x, view_bounds.left + border_distance);
	player1Pos.x = std::min(player1Pos.x, view_bounds.left + view_bounds.width - border_distance);
	player1Pos.y = std::max(player1Pos.y, view_bounds.top + border_distance);
	player1Pos.y = std::min(player1Pos.y, view_bounds.top + view_bounds.height - border_distance);
	m_player_tank->setPosition(player1Pos);
	   
	// Keep Player 2 Tank inside the screen / bounds
	sf::Vector2f player2Pos = m_player_tank_2->getPosition();
	player2Pos.x = std::max(player2Pos.x, view_bounds.left + border_distance);
	player2Pos.x = std::min(player2Pos.x, view_bounds.left + view_bounds.width - border_distance);
	player2Pos.y = std::max(player2Pos.y, view_bounds.top + border_distance);
	player2Pos.y = std::min(player2Pos.y, view_bounds.top + view_bounds.height - border_distance);
	m_player_tank_2->setPosition(player2Pos);
}

void World::AdaptPlayerVelocity()
{
	sf::Vector2f player1Vel = m_player_tank->GetVelocity();
	sf::Vector2f player2Vel = m_player_tank_2->GetVelocity();

	// If Player 1 moving diagonally then reduce velocity
	if (player1Vel.x != 0.f && player1Vel.y != 0.f)
	{
		m_player_tank->SetVelocity(player1Vel / std::sqrt(2.f));
	}

	// If Player 2 moving diagonally then reduce velocity
	if (player2Vel.x != 0.f && player2Vel.y != 0.f)
	{
		m_player_tank_2->SetVelocity(player2Vel / std::sqrt(2.f));
	}

}

sf::FloatRect World::GetViewBounds() const
{
	return sf::FloatRect(m_camera.getCenter() - m_camera.getSize() / 2.f, m_camera.getSize());
}

sf::FloatRect World::GetBattlefieldBounds() const
{
	//Return camera bounds + a small area at the top where enemies spawn offscreen
	sf::FloatRect bounds = GetViewBounds();
	bounds.top -= 100.f;
	bounds.height += 100.f;

	return bounds;
}

void World::SpawnEnemies()
{
	//Spawn an enemy when they are relevant - they are relevant when they enter the battlefield bounds
	while (!m_enemy_spawn_points.empty() && m_enemy_spawn_points.back().m_y > GetBattlefieldBounds().top)
	{
		SpawnPoint spawn = m_enemy_spawn_points.back();
		std::unique_ptr<Tank> enemy(new Tank(spawn.m_type, TankType::kCannonCamo, m_textures, m_fonts));
		enemy->setPosition(spawn.m_x, spawn.m_y);
		enemy->setRotation(180.f);
		m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(enemy));

		m_enemy_spawn_points.pop_back();

	}
}

void World::AddEnemy(TankType type, float relX, float relY)
{
	SpawnPoint spawn(type, m_spawn_position.x + relX, m_spawn_position.y - relY);
	m_enemy_spawn_points.emplace_back(spawn);
}

void World::AddEnemies()
{
	////Add all enemies
	//AddEnemy(TankType::kSand, 0.f, 500.f);
	//AddEnemy(TankType::kSand, 0.f, 1000.f);
	//AddEnemy(TankType::kSand, 100.f, 1100.f);
	//AddEnemy(TankType::kSand, -100.f, 1100.f);
	//AddEnemy(TankType::kGreen, -70.f, 1400.f);
	//AddEnemy(TankType::kGreen, 70.f, 1400.f);
	//AddEnemy(TankType::kGreen, 70.f, 1600.f);

	////Sort according to y value so that lower enemies are checked first
	//std::sort(m_enemy_spawn_points.begin(), m_enemy_spawn_points.end(), [](SpawnPoint lhs, SpawnPoint rhs)
	//	{
	//		return lhs.m_y < rhs.m_y;
	//	});
}

void World::GuideMissiles()
{
	// Setup command that stores all enemies in mActiveEnemies
	Command enemyCollector;
	enemyCollector.category = Category::kEnemyTank;
	enemyCollector.action = DerivedAction<Tank>([this](Tank& enemy, sf::Time)
		{
			if (!enemy.IsDestroyed())
				m_active_enemies.push_back(&enemy);
		});

	// Setup command that guides all missiles to the enemy which is currently closest to the player
	Command missileGuider;
	missileGuider.category = Category::kAlliedProjectile;
	missileGuider.action = DerivedAction<Projectile>([this](Projectile& missile, sf::Time)
		{
			// Ignore unguided bullets
			if (!missile.IsGuided())
				return;

			float minDistance = std::numeric_limits<float>::max();
			Tank* closestEnemy = nullptr;

			// Find closest enemy
			for (Tank* enemy : m_active_enemies)
			{
				float enemyDistance = Distance(missile, *enemy);

				if (enemyDistance < minDistance)
				{
					closestEnemy = enemy;
					minDistance = enemyDistance;
				}
			}

			if (closestEnemy)
				missile.GuideTowards(closestEnemy->GetWorldPosition());
		});

	// Push commands, reset active enemies
	m_command_queue.Push(enemyCollector);
	m_command_queue.Push(missileGuider);
	m_active_enemies.clear();
}

bool MatchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();
	//std::cout << category1 << category2 << std::endl;

	if (type1 & category1 && type2 & category2)
	{
		return true;
	}
	else if (type1 & category2 && type2 & category1)
	{
		std::swap(colliders.first, colliders.second);
		return true;
	}
	else
	{
		return false;
	}
}

void World::HandleCollisions()
{
	std::set<SceneNode::Pair> collision_pairs;
	m_scenegraph.CheckSceneCollision(m_scenegraph, collision_pairs);
	for (SceneNode::Pair pair : collision_pairs)
	{
		if (MatchesCategories(pair, Category::Type::kPlayerTank, Category::Type::kPlayer2Tank))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& player2 = static_cast<Tank&>(*pair.second);

			sf::Vector2f player1Pos = player.getPosition();
			sf::Vector2f player2Pos = player2.getPosition();

			//Collision
			if (player1Pos.x < player2Pos.x)
			{
				m_player_tank->move(-1.0f, 0.0f);
				m_player_tank_2->move(1.0f, 0.0f);
			}
			if (player1Pos.x > player2Pos.x)
			{
				m_player_tank->move(1.0f, 0.0f);
				m_player_tank_2->move(-1.0f, 0.0f);
			}
			if (player1Pos.y < player2Pos.y)
			{
				m_player_tank->move(0.0f, -1.0f);
				m_player_tank_2->move(0.0f, 1.0f);
			}
			if (player1Pos.y > player2Pos.y)
			{
				m_player_tank->move(0.0f, 1.0f);
				m_player_tank_2->move(0.0f, -1.0f);
			}

			// Damage Player Tanks 
			player.Damage(1.0f);
			player2.Damage(1.0f);

		}
		else if (MatchesCategories(pair, Category::Type::kPlayerTank, Category::Type::kPickup))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			//Apply the pickup effect
			pickup.Apply(player);
			pickup.Destroy();
		}
		else if (MatchesCategories(pair, Category::Type::kPlayer2Tank, Category::Type::kPickup))
		{
			auto& player2 = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			//Apply the pickup effect
			pickup.Apply(player2);
			pickup.Destroy();
		}
		else if (MatchesCategories(pair, Category::Type::kPlayerTank, Category::Type::kAlliedProjectile) 
			|| MatchesCategories(pair, Category::Type::kPlayer2Tank, Category::Type::kAlliedProjectile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);

			//Apply the projectile damage to the tank
			tank.Damage(projectile.GetDamage());
			projectile.Destroy();
		}
	}
}

void World::DestroyEntitiesOutsideView()
{
	Command command;
	command.category = Category::Type::kEnemyTank | Category::Type::kProjectile;
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time)
		{
			//Does the object intersect with the battlefield
			if (!GetBattlefieldBounds().intersects(e.GetBoundingRect()))
			{
				std::cout << "Destroying the entity" << std::endl;
				e.Destroy();
			}
		});
	m_command_queue.Push(command);
}