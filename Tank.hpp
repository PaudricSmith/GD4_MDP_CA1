#pragma once
#include "Entity.hpp"
#include "TankType.hpp"
#include "ResourceIdentifiers.hpp"

#include <SFML/Graphics/Sprite.hpp>

#include "CommandQueue.hpp"
#include "ProjectileType.hpp"
#include "TextNode.hpp"


class Tank : public Entity
{
public:
	Tank(TankType type, TankType cannonType, const TextureHolder& textures, const FontHolder& fonts);
	unsigned int GetCategory() const override;

	void DisablePickups();
	int GetIdentifier();
	void SetIdentifier(int identifier);
	int GetMissileAmmo() const;
	void SetMissileAmmo(int ammo);

	void IncreaseFireRate();
	void IncreaseSpread();
	void CollectMissiles(unsigned int count);
	void UpdateTexts();
	void UpdateMovementPattern(sf::Time dt);
	void RotateCannon(float angle);
	void MoveSoundPlayInterval();
	void CannonMoveSoundPlayInterval();
	void Fire();
	void LaunchMissile();
	void CreateBullets(SceneNode& node, const TextureHolder& textures) const;
	void CreateProjectile(SceneNode& node, ProjectileType type, float x_offset, float y_offset, const TextureHolder& textures) const;

	float GetMaxSpeed() const;
	float GetRotationSpeed() const;
	float GetCannonRotationSpeed() const;
	float GetCannonRotationAngle() const;
	TankType GetTankType() const;

	sf::FloatRect GetBoundingRect() const override;
	bool IsMarkedForRemoval() const override;
	void PlayLocalSound(CommandQueue& commands, SoundEffects effect);


private:
	void DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const override;
	void UpdateCurrent(sf::Time dt, CommandQueue& commands) override;

	void CheckProjectileLaunch(sf::Time dt, CommandQueue& commands);
	bool IsPlayerTank() const;
	void CreatePickup(SceneNode& node, const TextureHolder& textures) const;
	void CheckPickupDrop(CommandQueue& commands);


private:
	TankType m_type;
	TankType m_cannon_type;
	
	sf::Sprite m_sprite;
	sf::Sprite m_cannon_sprite;
	sf::Time m_fire_countdown;
	sf::Time m_move_sound_countdown;
	sf::Time m_move_cannon_sound_countdown;

	TextNode* m_health_display;
	TextNode* m_missile_display;

	bool m_is_firing;
	bool m_is_launching_missile;
	bool m_pickups_enabled;
	bool m_is_marked_for_removal;
	bool m_played_explosion_sound;
	bool m_is_playing_move_sound;
	bool m_is_playing_cannon_move_sound;

	float m_travelled_distance;
	float m_cannon_rotation;

	int m_directions_index;
	int m_identifier;

	unsigned int m_fire_rate;
	unsigned int m_spread_level;
	unsigned int m_missile_ammo;

	Command m_fire_command;
	Command m_missile_command;
	Command m_drop_pickup_command;
};