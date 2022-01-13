#pragma once

#include <functional>
#include <vector>
#include <map>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Event.hpp>

#include "ResourceIdentifiers.hpp"
#include "Category.hpp"
#include "PlayerActions.hpp"
#include "Category.hpp"
#include "Player.hpp"


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
	sf::Time m_move_sound_interval;
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

struct PlayerData
{
	Category::Type playerCategory;

	std::map<sf::Keyboard::Key, PlayerActions> m_key_binding;
};

std::vector<TankData> InitializeTankData();
std::vector<ProjectileData> InitializeProjectileData();
std::vector<PickupData> InitializePickupData();
std::vector<PlayerData> InitializePlayerData();