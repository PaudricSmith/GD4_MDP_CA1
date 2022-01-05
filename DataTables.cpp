#include "DataTables.hpp"
#include "TankType.hpp"
#include "Tank.hpp"
#include "PickupType.hpp"
#include "ProjectileType.hpp"

std::vector<TankData> InitializeTankData()
{
	std::vector<TankData> data(static_cast<int>(TankType::kTankCount));

	data[static_cast<int>(TankType::kCamo)].m_hitpoints = 100;
	data[static_cast<int>(TankType::kCamo)].m_speed = 200.f;
	//data[static_cast<int>(TankType::kCamo)].m_rotationSpeed = 1.f;
	data[static_cast<int>(TankType::kCamo)].m_fire_interval = sf::seconds(1);
	data[static_cast<int>(TankType::kCamo)].m_texture = Textures::kCamo;

	data[static_cast<int>(TankType::kSand)].m_hitpoints = 20;
	data[static_cast<int>(TankType::kSand)].m_speed = 80.f;
	data[static_cast<int>(TankType::kSand)].m_fire_interval = sf::Time::Zero;
	data[static_cast<int>(TankType::kSand)].m_texture = Textures::kSand;

	//AI
	data[static_cast<int>(TankType::kSand)].m_directions.emplace_back(Direction(+45.f, 80.f));
	data[static_cast<int>(TankType::kSand)].m_directions.emplace_back(Direction(-45.f, 160.f));
	data[static_cast<int>(TankType::kSand)].m_directions.emplace_back(Direction(+45.f, 80.f));

	data[static_cast<int>(TankType::kGreen)].m_hitpoints = 40;
	data[static_cast<int>(TankType::kGreen)].m_speed = 50.f;
	data[static_cast<int>(TankType::kGreen)].m_fire_interval = sf::seconds(2);
	data[static_cast<int>(TankType::kGreen)].m_texture = Textures::kGreen;
	//AI
	data[static_cast<int>(TankType::kGreen)].m_directions.emplace_back(Direction(+45.f, 50.f));
	data[static_cast<int>(TankType::kGreen)].m_directions.emplace_back(Direction(0.f, 50.f));
	data[static_cast<int>(TankType::kGreen)].m_directions.emplace_back(Direction(-45.f, 100.f));
	data[static_cast<int>(TankType::kGreen)].m_directions.emplace_back(Direction(0.f, 50.f));
	data[static_cast<int>(TankType::kGreen)].m_directions.emplace_back(Direction(+45.f, 50.f));
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

	data[static_cast<int>(ProjectileType::kMissile)].m_damage = 200;
	data[static_cast<int>(ProjectileType::kMissile)].m_speed = 100.f;
	data[static_cast<int>(ProjectileType::kMissile)].m_texture = Textures::kMissile;
	return data;
}


std::vector<PickupData> InitializePickupData()
{
	std::vector<PickupData> data(static_cast<int>(PickupType::kPickupCount));

	data[static_cast<int>(PickupType::kHealthRefill)].m_texture = Textures::kHealthRefill;
	data[static_cast<int>(PickupType::kHealthRefill)].m_action = [](Tank& a) {a.Repair(25); };

	data[static_cast<int>(PickupType::kMissileRefill)].m_texture = Textures::kMissileRefill;
	data[static_cast<int>(PickupType::kMissileRefill)].m_action = std::bind(&Tank::CollectMissiles, std::placeholders::_1, 3);

	data[static_cast<int>(PickupType::kFireSpread)].m_texture = Textures::kFireSpread;
	data[static_cast<int>(PickupType::kFireSpread)].m_action = std::bind(&Tank::IncreaseSpread, std::placeholders::_1);

	data[static_cast<int>(PickupType::kFireRate)].m_texture = Textures::kFireRate;
	data[static_cast<int>(PickupType::kFireRate)].m_action = std::bind(&Tank::IncreaseFireRate, std::placeholders::_1);
	return data;
}