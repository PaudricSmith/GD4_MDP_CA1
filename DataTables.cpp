#include "DataTables.hpp"
#include "TankType.hpp"
#include "Tank.hpp"
#include "PickupType.hpp"
#include "ProjectileType.hpp"


std::vector<TankData> InitializeTankData()
{
	// Initialize vector size with Tank count
	std::vector<TankData> data(static_cast<int>(TankType::kTankCount));

	// Player 1 Tank
	data[static_cast<int>(TankType::kCamo)].m_hitpoints = 300;
	data[static_cast<int>(TankType::kCamo)].m_speed = 50.f;
	data[static_cast<int>(TankType::kCamo)].m_rotation_speed = 1.f;
	data[static_cast<int>(TankType::kCamo)].m_cannon_rotation_speed = 1.f;
	data[static_cast<int>(TankType::kCamo)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(TankType::kCamo)].m_move_sound_interval = sf::seconds(0.35);
	data[static_cast<int>(TankType::kCamo)].m_move_cannon_sound_interval = sf::seconds(0.35);
	data[static_cast<int>(TankType::kCamo)].m_texture = Textures::kCamo;
	// Set the Tanks Cannon texture
	data[static_cast<int>(TankType::kCannonCamo)].m_cannon_texture = Textures::kCannonCamo;

	// Player 2 Tank
	data[static_cast<int>(TankType::kSand)].m_hitpoints = 300;
	data[static_cast<int>(TankType::kSand)].m_speed = 50.f;
	data[static_cast<int>(TankType::kSand)].m_rotation_speed = 1.f;
	data[static_cast<int>(TankType::kSand)].m_cannon_rotation_speed = 1.f;
	data[static_cast<int>(TankType::kSand)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(TankType::kSand)].m_move_sound_interval = sf::seconds(0.35);
	data[static_cast<int>(TankType::kSand)].m_move_cannon_sound_interval = sf::seconds(0.35);
	data[static_cast<int>(TankType::kSand)].m_texture = Textures::kSand;
	// Set the Tanks Cannon texture
	data[static_cast<int>(TankType::kCannonSand)].m_cannon_texture = Textures::kCannonSand;

	return data;
}

std::vector<ProjectileData> InitializeProjectileData()
{
	std::vector<ProjectileData> data(static_cast<int>(ProjectileType::kProjectileCount));

	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_speed = 200;
	data[static_cast<int>(ProjectileType::kAlliedBullet)].m_texture = Textures::kBullet;

	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_damage = 10;
	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_speed = 200;
	data[static_cast<int>(ProjectileType::kEnemyBullet)].m_texture = Textures::kBullet;

	data[static_cast<int>(ProjectileType::kMissile)].m_damage = 35;
	data[static_cast<int>(ProjectileType::kMissile)].m_speed = 150.f;
	data[static_cast<int>(ProjectileType::kMissile)].m_texture = Textures::kMissile;
	return data;
}


std::vector<PickupData> InitializePickupData()
{
	std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));

	data[static_cast<int>(PickupType::kHealthRefill)].m_texture = Textures::kHealthRefill;
	data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Tank& a) {a.Repair(100); };

	data[static_cast<int>(PickupType::kMissileRefill)].m_texture = Textures::kMissileRefill;
	data[static_cast<int>(PickupType::kMissileRefill)].m_action = std::bind(&Tank::CollectMissiles, std::placeholders::_1, 3);

	data[static_cast<int>(PickupType::kFireRate)].m_texture = Textures::kFireRate;
	data[static_cast<int>(PickupType::kFireRate)].m_action = std::bind(&Tank::IncreaseFireRate, std::placeholders::_1);

	data[static_cast<int>(PickupType::kExtraBullets)].m_texture = Textures::kExtraBullets;
	data[static_cast<int>(PickupType::kExtraBullets)].m_action = std::bind(&Tank::IncreaseBullets, std::placeholders::_1);

	data[static_cast<int>(PickupType::kTankSpeedIncrease)].m_texture = Textures::kTankSpeedIncrease;
	data[static_cast<int>(PickupType::kTankSpeedIncrease)].m_action = std::bind(&Tank::IncreaseTankSpeed, std::placeholders::_1);

	return data;
}