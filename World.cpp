#include "World.hpp"

#include <SFML/Graphics/RenderWindow.hpp>
#include <iostream>
#include <limits>

#include "Pickup.hpp"
#include "Projectile.hpp"
#include "Utility.hpp"
#include "SoundNode.hpp"

World::World(sf::RenderTarget& output_target, const TextureHolder& textures, FontHolder& font, SoundPlayer& sounds, bool networked)
	: m_target(output_target)
	, m_camera(output_target.getDefaultView())
	, m_textures()
	, m_fonts(font)
	, m_sfx_player(sounds)
	, m_scenegraph()
	, m_scene_layers()
	, m_world_bounds(0.f, 0.f, m_camera.getSize().x, m_camera.getSize().y)
	, m_spawn_position(m_camera.getSize().x / 2.f, m_world_bounds.height - m_camera.getSize().y / 2.f)
	, m_scrollspeed(0.f)
	, m_scrollspeed_compensation(0.f)
	, m_player_tank()
	, m_is_pickups_spawned(false)
	, m_enemy_spawn_points()
	, m_active_enemies()
	, m_wall_bounds()
	, m_networked_world(networked)
	, m_network_node(nullptr)
	, m_finish_sprite(nullptr)
{
	LoadTextures();
	BuildScene();
	m_camera.setCenter(m_spawn_position);

	// Pickup Node
	m_drop_pickup_command.category = static_cast<int>(Category::Type::kScene);
	m_drop_pickup_command.action = [this, &textures](SceneNode& node, sf::Time)
	{
		CreatePickups(node, textures);
	};

	PlaceWalls();

	m_sfx_player.Play(SoundEffects::kToastBeep2);
}

///
/// Loïc Dornel, D00243372
///	<summary>
///	Places all walls at the start of the game
///	</summary>
///	<pa
void World::PlaceWalls()
{
	std::unique_ptr<Wall> wallTopPtr(new Wall(sf::Vector2f(512.f, .75f), 0.1f, 1.5f, m_textures));
	m_wall_bounds.push_back((*wallTopPtr).GetBoundingRect());
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(wallTopPtr));

	std::unique_ptr<Wall> wallBottomPtr(new Wall(sf::Vector2f(512.f, 750.f), 0.1f, 1.5f, m_textures));
	m_wall_bounds.push_back((*wallBottomPtr).GetBoundingRect());
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(wallBottomPtr));

	std::unique_ptr<Wall> wallLeftTopPtr(new Wall(sf::Vector2f(256.f, 256.f), 0.1f, .5f, m_textures));
	m_wall_bounds.push_back((*wallLeftTopPtr).GetBoundingRect());
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(wallLeftTopPtr));

	std::unique_ptr<Wall> wallLeftBottomPtr(new Wall(sf::Vector2f(256.f, 512.f), 0.1f, .5f, m_textures));
	m_wall_bounds.push_back((*wallLeftBottomPtr).GetBoundingRect());
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(wallLeftBottomPtr));

	std::unique_ptr<Wall> wallRightTopPtr(new Wall(sf::Vector2f(768.f, 256.f), 0.1f, .5f, m_textures));
	m_wall_bounds.push_back((*wallRightTopPtr).GetBoundingRect());
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(wallRightTopPtr));

	std::unique_ptr<Wall> wallRightBottomPtr(new Wall(sf::Vector2f(768.f, 512.f), 0.1f, .5f, m_textures));
	m_wall_bounds.push_back((*wallRightBottomPtr).GetBoundingRect());
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(wallRightBottomPtr));
}


///
/// Paudric Smith, D00215637
/// <summary>
/// Creates 5 Random Pickups at the start of the game.
/// 4 in each corner and 1 in the middle.
/// The 4 corner pickups gradually move off screen towards their corner.
/// </summary>
/// <param name="node"></param>
/// <param name="textures"></param>
void World::CreatePickups(SceneNode& node, const TextureHolder& textures) const
{
	sf::Vector2f topLeftCorner = sf::Vector2f(100, 100);
	sf::Vector2f topRightCorner = sf::Vector2f(924, 100);
	sf::Vector2f bottomLeftCorner = sf::Vector2f(100, 668);
	sf::Vector2f bottomRightCorner = sf::Vector2f(924, 668);
	sf::Vector2f middle = sf::Vector2f(512, 385);

	auto randType1 = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup1(new Pickup(randType1, m_textures));
	pickup1->setPosition(topLeftCorner);
	pickup1->SetVelocity(-2.0f, -2.0f);
	node.AttachChild(std::move(pickup1));

	auto randType2 = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup2(new Pickup(randType2, m_textures));
	pickup2->setPosition(topRightCorner);
	pickup2->SetVelocity(2.0f, -2.0f);
	node.AttachChild(std::move(pickup2));

	auto randType3 = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup3(new Pickup(randType3, m_textures));
	pickup3->setPosition(bottomLeftCorner);
	pickup3->SetVelocity(-2.0f, 2.0f);
	node.AttachChild(std::move(pickup3));

	auto randType4 = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup4(new Pickup(randType4, m_textures));
	pickup4->setPosition(bottomRightCorner);
	pickup4->SetVelocity(2.0f, 2.0f);
	node.AttachChild(std::move(pickup4));

	auto randType5 = static_cast<PickupType>(Utility::RandomInt(static_cast<int>(PickupType::kPickupCount)));
	std::unique_ptr<Pickup> pickup5(new Pickup(randType5, m_textures));
	pickup5->setPosition(middle);
	pickup5->SetVelocity(0.0f, 0.0f);
	node.AttachChild(std::move(pickup5));
}

void World::SetWorldScrollCompensation(float compensation)
{
	m_scrollspeed_compensation = compensation;
}

void World::Update(sf::Time dt)
{

	// Spawn Pickups at start of game
	if (m_is_pickups_spawned == false)
	{
		CommandQueue& commands = GetCommandQueue();
		commands.Push(m_drop_pickup_command);

		m_is_pickups_spawned = true;
	}

	/*m_player_tank->SetVelocity(0.f, 0.f);*/

	DestroyEntitiesOutsideView();

	//Forward commands to the scenegraph until the command queue is empty-
	while (!m_command_queue.IsEmpty())
	{
		m_scenegraph.OnCommand(m_command_queue.Pop(), dt);
	}

	AdaptPlayerVelocity();

	HandleCollisions();
	//RemoveWrecks() only destroys the entities, not the pointers in m_player_tank
	auto first_to_remove = std::remove_if(m_player_tank.begin(), m_player_tank.end(), std::mem_fn(&Tank::IsMarkedForRemoval));
	m_player_tank.erase(first_to_remove, m_player_tank.end());
	m_scenegraph.RemoveWrecks();


	//Apply movement
	m_scenegraph.Update(dt, m_command_queue);
	AdaptPlayerPosition();

	UpdateSounds();
}

void World::Draw()
{
	m_target.setView(m_camera);
	m_target.draw(m_scenegraph);
}

Tank* World::GetTank(int identifier) const
{
	for (Tank* a : m_player_tank)
	{
		if (a->GetIdentifier() == identifier)
		{
			return a;
		}
	}
	return nullptr;
}

void World::RemoveTank(int identifier)
{
	Tank* tank = GetTank(identifier);
	if (tank)
	{
		tank->Destroy();
		m_player_tank.erase(std::find(m_player_tank.begin(), m_player_tank.end(), tank));
	}
}

/// <summary>
/// Separate Tank into either Green or Yellow team depending on its identifier being odd or even
/// Green tanks X position is set to the left of screen centre and it's Y is multiplied by it's identifier to make a row
/// Yellow tanks X position is set to the right of screen centre and it's Y is multiplied by it's identifier and divided by 2 to make a row
/// </summary>
/// <param name="identifier"></param>
/// <returns></returns>
Tank* World::AddTank(int identifier)
{
	std::cout << "identifier ****************************************************************************************: " << identifier << std::endl;

	bool greenTeam = true;
	TankType tankBaseType;
	TankType tankCannonType;

	// Check identifier if it is an even number, if so then the player is on the Yellow Team.
	if (identifier % 2 == 0)
		greenTeam = false;	

	// Player spawn position depending on which colour the Tank is
	sf::Vector2f playerPosition;
	if (greenTeam)
	{
		tankBaseType = TankType::kCamo;
		tankCannonType = TankType::kCannonCamo;
		playerPosition = sf::Vector2f(350, 50 * identifier - 350);
	}
	else
	{
		tankBaseType = TankType::kSand;
		tankCannonType = TankType::kCannonSand;
		playerPosition = sf::Vector2f(-350, 50 * identifier - 400);
	}


	std::unique_ptr<Tank> player(new Tank(tankBaseType, tankCannonType, m_textures, m_fonts));
	player->setPosition(m_camera.getCenter() - playerPosition);
	if (greenTeam)
	{
		player->setRotation(90);
		//player->SetCannonAngle(0);
	}
	else
	{
		player->setRotation(-90);
		//player->SetCannonAngle(0);
	}
	player->SetIdentifier(identifier);
	m_player_tank.emplace_back(player.get());
	m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(player));

	return m_player_tank.back();

}

void World::CreatePickup(sf::Vector2f position, PickupType type)
{
	std::unique_ptr<Pickup> pickup(new Pickup(type, m_textures));
	pickup->setPosition(position);
	pickup->SetVelocity(0.f, 1.f);
	m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(pickup));
}

bool World::PollGameAction(GameActions::Action& out)
{
	return m_network_node->PollGameAction(out);
}

void World::SetCurrentBattleFieldPosition(float lineY)
{
	m_camera.setCenter(m_camera.getCenter().x, lineY - m_camera.getSize().y / 2);
	m_spawn_position.y = m_world_bounds.height;
}

void World::SetWorldHeight(float height)
{
	m_world_bounds.height = height;
}

//bool World::HasAlivePlayer1() const
//{
//	// If a Player 1 Tank is not destroyed, hence Player 1 is still alive
//	return !m_player_tank.empty();
//}

//bool World::HasAlivePlayer2() const
//{
//	// If a Player 2 Tank is not destroyed, hence Player 2 is still alive
//	return !m_player_tank_2->IsMarkedForRemoval();
//}

void World::LoadTextures()
{	
	// Tank images from Game Supply
	// https://gamesupply.itch.io/huge-universal-game-asset
	// Player 1 textures
	m_textures.Load(Textures::kCamo, "Media/Textures/Tank/Camo Tank/Camo Tank Base.png"); // Tank base
	m_textures.Load(Textures::kCannonCamo, "Media/Textures/Tank/Camo Tank/Camo Tank Turret.png"); // Tank cannon
	
	// Player 2 textures
	m_textures.Load(Textures::kSand, "Media/Textures/Tank/sand/Sand Tank Base.png"); // Tank base
	m_textures.Load(Textures::kCannonSand, "Media/Textures/Tank/sand/Sand Tank Turret.png"); // Tank cannon
	
	// Other Tank textures
	m_textures.Load(Textures::kGreen, "Media/Textures/Tank/green/Tank_B_Big_Green_2_128x194.png"); // base and cannon combined
	m_textures.Load(Textures::kBrown, "Media/Textures/Tank/brown/Tank_Big_Brown_128x194.png"); // base and cannon combined

	// Level1BG.png was made from an image using this package
	// https://gamesupply.itch.io/ultimate-space-game-mega-asset-package
	// Background textures
	m_textures.Load(Textures::kLevel1BG, "Media/Textures/Level1BG.png");

	// // https://gamesupply.itch.io/ultimate-space-game-mega-asset-package
	// Middle Wall
	m_textures.Load(Textures::kMiddleWall, "Media/Textures/WallGraySepia.jpg");

	// Images below came from SFML Game Development book Github
	// https://github.com/SFML/SFML-Game-Development-Book
	// Projectile textures
	m_textures.Load(Textures::kBullet, "Media/Textures/Bullet.png");
	m_textures.Load(Textures::kMissile, "Media/Textures/Missile.png");
	m_textures.Load(Textures::kHealthRefill, "Media/Textures/HealthRefill.png");
	m_textures.Load(Textures::kMissileRefill, "Media/Textures/MissileRefill.png");
	m_textures.Load(Textures::kExtraBullets, "Media/Textures/FireSpread.png");
	m_textures.Load(Textures::kFireRate, "Media/Textures/FireRate.png");

	m_textures.Load(Textures::kObstacleWall, "Media/Textures/bg308.png");
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
	sf::Texture& texture = m_textures.Get(Textures::kLevel1BG);
	sf::IntRect textureRect(m_world_bounds);
	//Tile the texture to cover our world
	texture.setRepeated(true);

	//Add the background sprite to our scene
	std::unique_ptr<SpriteNode> background_sprite(new SpriteNode(texture, textureRect));
	background_sprite->setPosition(m_world_bounds.left, m_world_bounds.top + 5);
	m_scene_layers[static_cast<int>(Layers::kBackground)]->AttachChild(std::move(background_sprite));


	// Add sound effect node for Players
	std::unique_ptr<SoundNode> soundNode(new SoundNode(m_sfx_player));
	m_scenegraph.AttachChild(std::move(soundNode));


	////Add Player 1 Tank
	//std::unique_ptr<Tank> leader(new Tank(TankType::kCamo, TankType::kCannonCamo, m_textures, m_fonts));
	//m_player_tank = leader.get();
	//m_player_tank->setPosition(m_spawn_position.x - 200.0f, m_spawn_position.y);
	//m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(leader));

	////Add Player 2 Tank
	//std::unique_ptr<Tank> leader2(new Tank(TankType::kSand, TankType::kCannonSand, m_textures, m_fonts));
	//m_player_tank_2 = leader2.get();
	//m_player_tank_2->setPosition(m_spawn_position.x + 200.0f, m_spawn_position.y);
	//m_scene_layers[static_cast<int>(Layers::kAir)]->AttachChild(std::move(leader2));

	if (m_networked_world)
	{
		std::unique_ptr<NetworkNode> network_node(new NetworkNode());
		m_network_node = network_node.get();
		m_scenegraph.AttachChild(std::move(network_node));
	}

}

CommandQueue& World::GetCommandQueue()
{
	return m_command_queue;
}

void World::AdaptPlayerPosition()
{
	sf::FloatRect view_bounds = GetViewBounds();
	const float border_distance = 40.f;

	// Keep Tanks inside the screen / bounds
	for (Tank* tank : m_player_tank)
	{
		sf::Vector2f position = tank->getPosition();
		position.x = std::max(position.x, view_bounds.left + border_distance);
		position.x = std::min(position.x, view_bounds.left + view_bounds.width - border_distance);
		position.y = std::max(position.y, view_bounds.top + border_distance);
		position.y = std::min(position.y, view_bounds.top + view_bounds.height - border_distance);

		tank->setPosition(position);
	}

}

void World::AdaptPlayerVelocity()
{
	/*sf::Vector2f player1Vel = m_player_tank->GetVelocity();
	sf::Vector2f player2Vel = m_player_tank_2->GetVelocity();*/

	//// If Player 1 moving diagonally then reduce velocity
	//if (player1Vel.x != 0.f && player1Vel.y != 0.f)
	//{
	//	m_player_tank->SetVelocity(player1Vel / std::sqrt(2.f));
	//}

	//// If Player 2 moving diagonally then reduce velocity
	//if (player2Vel.x != 0.f && player2Vel.y != 0.f)
	//{
	//	m_player_tank_2->SetVelocity(player2Vel / std::sqrt(2.f));
	//}

	for (Tank* tank : m_player_tank)
	{
		sf::Vector2f velocity = tank->GetVelocity();
		//if moving diagonally then reduce velocity
		if (velocity.x != 0.f && velocity.y != 0.f)
		{
			// Wall collision
			sf::FloatRect player_bounds = tank->GetBoundingRect();
			sf::FloatRect next_pos = player_bounds;

			// Based on a tutorial by Suraj Sharma
			// https://www.youtube.com/watch?v=QM92txFYjLI
			// https://www.youtube.com/watch?v=A04MPkBL5H4
			
			for (auto& wall_bound : m_wall_bounds)
			{
				next_pos.left += velocity.x;
				next_pos.top += velocity.y;

				if (wall_bound.intersects(next_pos))
				{
					const float player_left = player_bounds.left, player_right = player_bounds.left + player_bounds.width;
					const float player_top = player_bounds.top, player_bottom = player_bounds.top + player_bounds.height;
					const float player_width = player_bounds.width, player_height = player_bounds.height;

					const float wall_left = wall_bound.left, wall_right = wall_bound.left + wall_bound.width;
					const float wall_top = wall_bound.top, wall_bottom = wall_bound.top + wall_bound.height;

					// Horizontal collision
					if (player_top < wall_bottom && player_bottom > wall_top)
					{
						velocity.x = 0.f;

						// Right collision
						if (player_left < wall_left && player_right < wall_right)
						{
							std::cout << "Right collision" << std::endl;
							//tank->setPosition(wall_left - player_width, player_top);
						}

						// Left collision
						if (player_left > wall_left && player_right > wall_right)
						{
							std::cout << "Left collision" << std::endl;
							//tank->setPosition(wall_left + player_width, player_top);
						}
					}

					// Vertical collision
					if (player_left < wall_right && player_right > wall_left)
					{
						velocity.y = 0.f;

						// Bottom collision
						if (player_top < wall_top && player_bottom < wall_bottom)
						{
							std::cout << "Bottom collision" << std::endl;
							//tank->setPosition(wall_left, player_top - player_height);
						}

						// Top collision
						if (player_top > wall_top && player_bottom > wall_bottom)
						{
							std::cout << "Top collision" << std::endl;
							//tank->setPosition(wall_left, player_top + player_height);
						}
					}
				}
			}

			tank->SetVelocity(velocity / std::sqrt(2.f));
		}
		//Add scrolling velocity
		tank->Accelerate(0.f, m_scrollspeed);
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

bool MatchesCategories(SceneNode::Pair& colliders, Category::Type type1, Category::Type type2)
{
	unsigned int category1 = colliders.first->GetCategory();
	unsigned int category2 = colliders.second->GetCategory();

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
		// If Player 1 Tank and Player 2 Tank collide
		if (MatchesCategories(pair, Category::Type::kPlayerTank, Category::Type::kPlayer2Tank))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& player2 = static_cast<Tank&>(*pair.second);

			sf::Vector2f player1Pos = player.getPosition();
			sf::Vector2f player2Pos = player2.getPosition();

			// Collision based on both Tanks x y positions, if there is a collision
			// in a certain direction then move the Tank in the opposite direction
			/*if (player1Pos.x < player2Pos.x)
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
			}*/

			// Damage Player Tanks a small bit when they collide
			player.Damage(1.0f);
			player2.Damage(1.0f);

			// Play Tank on Tank Collision SFX
			m_sfx_player.Play(SoundEffects::kTankHitTank);

		}
		// If Player 1 Tank collides with a PickUp
		else if (MatchesCategories(pair, Category::Type::kPlayerTank, Category::Type::kPickup))
		{
			auto& player = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			//Apply the pickup effect
			pickup.Apply(player);
			pickup.Destroy();

			// Play Good Pickup SFX
			m_sfx_player.Play(SoundEffects::kCollectGoodPickup);

		}
		// If Player 2 Tank collides with a PickUp
		else if (MatchesCategories(pair, Category::Type::kPlayer2Tank, Category::Type::kPickup))
		{
			auto& player2 = static_cast<Tank&>(*pair.first);
			auto& pickup = static_cast<Pickup&>(*pair.second);

			//Apply the pickup effect
			pickup.Apply(player2);
			pickup.Destroy();

			// Play Good Pickup SFX
			m_sfx_player.Play(SoundEffects::kCollectGoodPickup);

		}
		// If either Tank 1 or Tank 2 collide with a projectile
		else if (MatchesCategories(pair, Category::Type::kPlayerTank, Category::Type::kAlliedProjectile) 
			|| MatchesCategories(pair, Category::Type::kPlayer2Tank, Category::Type::kAlliedProjectile))
		{
			auto& tank = static_cast<Tank&>(*pair.first);
			auto& projectile = static_cast<Projectile&>(*pair.second);

			//Apply the projectile damage to the tank
			tank.Damage(projectile.GetDamage());


			if (projectile.IsGuided())
			{
				// Play Guided Missile hit SFX
				m_sfx_player.Play(SoundEffects::kGuidedMissileHit);
			}
			else
			{				
				// Play Normal bullet hit SFX
				m_sfx_player.Play(SoundEffects::kNormalBulletHit);
			}
			
			
			projectile.Destroy();
		}
	}
}

void World::DestroyEntitiesOutsideView()
{
	Command command;
	//command.category = Category::Type::kPlayerTank | Category::Type::kPlayer2Tank | Category::Type::kProjectile;
	command.category = Category::Type::kProjectile;
	command.action = DerivedAction<Entity>([this](Entity& e, sf::Time)
		{
			//Does the object intersect with the battlefield
			if (!GetViewBounds().intersects(e.GetBoundingRect()))
			{
				e.Destroy();
			}
		});
	m_command_queue.Push(command);
}

void World::UpdateSounds()
{	
	// Set listener's position to player
    //sf::Vector2f player1SoundPos = sf::Vector2f(500, 500);

	//m_sounds_2.SetListenerPosition(player2SoundPos);
	m_sfx_player.SetListenerPosition(m_spawn_position);
	
	// Remove unused sounds
	m_sfx_player.RemoveStoppedSounds();	
}