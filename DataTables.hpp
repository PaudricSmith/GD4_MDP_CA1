#pragma once
#include <functional>
#include <vector>
#include <SFML/System/Time.hpp>

#include "ResourceIdentifiers.hpp"

class Tank;

struct Direction
{
	Direction(float angle, float distance)
		: m_angle(angle), m_distance(distance)
	{
	}
	float m_angle;
	float m_distance;
};

struct TankData
{
	int m_hitpoints;
	float m_speed;
	float m_rotation_speed;
	float m_cannon_rotation_speed;
	Textures m_texture;
	Textures m_cannon_texture;
	sf::Time m_fire_interval;
	std::vector<Direction> m_directions;
};

struct ProjectileData
{
	int m_damage;
	float m_speed;
	Textures m_texture;
};

struct PickupData
{
	std::function<void(Tank&)> m_action;
	Textures m_texture;
};

std::vector<TankData> InitializeTankData();
std::vector<ProjectileData> InitializeProjectileData();
std::vector<PickupData> InitializePickupData();