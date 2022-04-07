#pragma once

#include "ResourceHolder.hpp"
#include "ResourceIdentifiers.hpp"
#include "SceneNode.hpp"
#include "SpriteNode.hpp"
#include "Tank.hpp"
#include "Layers.hpp"
#include "TankType.hpp"
#include "NetworkNode.hpp"

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include <array>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "CommandQueue.hpp"
#include "SoundPlayer.hpp"

#include "NetworkProtocol.hpp"
#include "PickupType.hpp"
#include "PlayerActions.hpp"

#include "Wall.hpp"

namespace sf
{
	class RenderTarget;
}


class World : private sf::NonCopyable
{
public:
	explicit World(sf::RenderTarget& output_target, const TextureHolder& textures, FontHolder& font, SoundPlayer& sounds, bool networked = false);
	void Update(sf::Time dt);
	void Draw();
	
	sf::FloatRect GetViewBounds() const;
	CommandQueue& GetCommandQueue();

	Tank* AddTank(int identifier);
	void RemoveTank(int identifier);
	void SetCurrentBattleFieldPosition(float line_y);
	void SetWorldHeight(float height);

	//void AddEnemy(TankType type, float rel_x, float rel_y);
	//void SortEnemies();
	bool HasAlivePlayer1() const;
	//bool HasAlivePlayer2() const;

	void SetWorldScrollCompensation(float compensation);
	Tank* GetTank(int identifier) const;
	sf::FloatRect GetBattlefieldBounds() const;
	void CreatePickup(sf::Vector2f position, PickupType type);
	bool PollGameAction(GameActions::Action& out);

private:
	void LoadTextures();
	void BuildScene();
	void AdaptPlayerPosition();
	void AdaptPlayerVelocity();
	void HandleCollisions();
	void DestroyEntitiesOutsideView();
	void UpdateSounds();

	void PlaceWalls();

	void CreatePickups(SceneNode& node, const TextureHolder& textures) const;

private:
	struct SpawnPoint
	{
		SpawnPoint(TankType type, float x, float y) : m_type(type), m_x(x), m_y(y)
		{

		}
		TankType m_type;
		float m_x;
		float m_y;
	};


private:
	sf::RenderTarget& m_target;
	sf::View m_camera;
	TextureHolder m_textures;
	FontHolder& m_fonts;
	SoundPlayer& m_sfx_player;
	SceneNode m_scenegraph;
	std::array<SceneNode*, static_cast<int>(Layers::kLayerCount)> m_scene_layers;
	CommandQueue m_command_queue;

	Command m_drop_pickup_command;

	sf::FloatRect m_world_bounds;
	sf::Vector2f m_spawn_position;
	float m_scrollspeed;
	float m_scrollspeed_compensation;
	std::vector<Tank*> m_player_tank;
	std::vector<SpawnPoint> m_enemy_spawn_points;
	std::vector<Tank*>	m_active_enemies;

	std::vector<sf::FloatRect> m_wall_bounds;

	bool m_is_pickups_spawned;

	bool m_networked_world;
	NetworkNode* m_network_node;
	SpriteNode* m_finish_sprite;

};