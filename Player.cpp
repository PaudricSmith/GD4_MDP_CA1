#include <algorithm>
#include <iostream>

#include "Player.hpp"
#include "Tank.hpp"
#include "NetworkProtocol.hpp"
#include <SFML/Network/Packet.hpp>
#include "Utility.hpp"
#include "DataTables.hpp"


//namespace
//{
//	const std::vector<PlayerData> Table = InitializePlayerData();
//}

struct TankMover
{
	TankMover(float vx, float vy, int identifier) : velocity(vx, vy)
	, tank_id(identifier)
	{}

	void operator()(Tank& tank, sf::Time) const
	{
		// Movement in direction of forward code based on this webpage
	    // https://www.cplusplus.com/forum/general/110659/ 
		
		if (tank.GetIdentifier() == tank_id)
		{
			float currentTankRotation = tank.getRotation();
			sf::Vector2f newTankPosition;

			// Moving forwards / backwards
			if (velocity.y > 0) // If up key pressed, get the x and y postion for the direction based on tank angle
			{
				newTankPosition.x = -sin(Utility::ToRadians(currentTankRotation));
				newTankPosition.y = cos(Utility::ToRadians(currentTankRotation));
			}
			else if (velocity.y < 0) // If down key pressed, same but invert the signs
			{
				newTankPosition.x = sin(Utility::ToRadians(currentTankRotation));
				newTankPosition.y = -cos(Utility::ToRadians(currentTankRotation));
			}

			tank.Accelerate(newTankPosition * tank.GetMaxSpeed());

			// Play Tank moving SFX 
			tank.MoveSoundPlayInterval();
		}	
	}

	sf::Vector2f velocity;
	int tank_id;
};

struct TankRotator // Rotate Tank
{
	TankRotator(float angle, int identifier) : rotation(angle)
	, tank_id(identifier)
	{}

	void operator()(Tank& tank, sf::Time) const
	{
		if (tank.GetIdentifier() == tank_id)
		{
			tank.Rotate(rotation * tank.GetRotationSpeed()); // Rotate Tank by sign and Tank rotation speed

			// Play Tank moving SFX 
			tank.MoveSoundPlayInterval();
		}	
	}

	float rotation;
	int tank_id;
};

struct CannonRotator // Rotate Cannon
{
	CannonRotator(float angle, int identifier) : rotation(angle)
	, tank_id(identifier)
	{}

	void operator()(Tank& tank, sf::Time) const
	{
		if (tank.GetIdentifier() == tank_id)
		{
			tank.RotateCannon(rotation * tank.GetCannonRotationSpeed()); // Rotate Tank Cannon by sign and cannon speed

			// Play Tank Cannon moving SFX 
			tank.CannonMoveSoundPlayInterval();
		}	
	}

	float rotation;
	int tank_id;
};

struct TankFireTrigger
{
	TankFireTrigger(int identifier)
	: tank_id(identifier)
	{
	}

	void operator() (Tank& tank, sf::Time) const
	{
		if (tank.GetIdentifier() == tank_id)
			tank.Fire();
	}

	int tank_id;
};

struct TankMissileTrigger
{
	TankMissileTrigger(int identifier)
	: tank_id(identifier)
	{
	}

	void operator() (Tank& tank, sf::Time) const
	{
		if (tank.GetIdentifier() == tank_id)
			tank.LaunchMissile();
	}

	int tank_id;
};

Player::Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding) 
	: m_key_binding(binding)
	, m_current_mission_status(MissionStatus::kMissionRunning)
	, m_identifier(identifier)
	, m_socket(socket)
{
	//Set initial action bindings
	InitialiseActions();

	// Assign all categories to player's tank
	for (auto& pair : m_action_binding)
	{
		// Green team are odd numbers, Yellow team are even numbers.
		if (identifier % 2 == 0)
		{
			pair.second.category = Category::kPlayer2Tank;
		}
		else
		{
			pair.second.category = Category::kPlayerTank;
		}
	}
		
}


void Player::HandleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		PlayerActions action;
		if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) && !IsRealtimeAction(action))
		{
			// Network connected -> send event over network
			if (m_socket)
			{
				sf::Packet packet;
				packet << static_cast<sf::Int32>(Client::PacketType::PlayerEvent);
				packet << m_identifier;
				packet << static_cast<sf::Int32>(action);
				m_socket->send(packet);
			}

			// Network disconnected -> local event
			else
			{
				commands.Push(m_action_binding[action]);
			}
		}
	}


	// Realtime change (network connected)
	if ((event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased) && m_socket)
	{
		
			PlayerActions action;
		if (m_key_binding && m_key_binding->CheckAction(event.key.code, action) && IsRealtimeAction(action))
		{
			// Send realtime change over network
			sf::Packet packet;
			packet << static_cast<sf::Int32>(Client::PacketType::PlayerRealtimeChange);
			packet << m_identifier;
			packet << static_cast<sf::Int32>(action);
			packet << (event.type == sf::Event::KeyPressed);
			m_socket->send(packet);
		}
	}
}

bool Player::IsLocal() const
{
	// No key binding means this player is remote
	return m_key_binding != nullptr;
}

void Player::DisableAllRealtimeActions()
{
	for (auto& action : m_action_proxies)
	{
		sf::Packet packet;
		packet << static_cast<sf::Int32>(Client::PacketType::PlayerRealtimeChange);
		packet << m_identifier;
		packet << static_cast<sf::Int32>(action.first);
		packet << false;
		m_socket->send(packet);
	}
}

void Player::HandleRealtimeInput(CommandQueue& commands)
{
	// Check if this is a networked game and local player or just a single player game
	if ((m_socket && IsLocal()) || !m_socket)
	{
		// Lookup all actions and push corresponding commands to queue
		std::vector<PlayerActions> activeActions = m_key_binding->GetRealtimeActions();
		for (PlayerActions action : activeActions)
			commands.Push(m_action_binding[action]);
	}
}

void Player::HandleRealtimeNetworkInput(CommandQueue& commands)
{
	if (m_socket && !IsLocal())
	{ 
		// Traverse all realtime input proxies. Because this is a networked game, the input isn't handled directly
		for (auto pair : m_action_proxies)
		{
			if (pair.second && IsRealtimeAction(pair.first))
				commands.Push(m_action_binding[pair.first]);
		}
	}
}

void Player::HandleNetworkEvent(PlayerActions action, CommandQueue& commands)
{
	commands.Push(m_action_binding[action]);
}

void Player::HandleNetworkRealtimeChange(PlayerActions action, bool actionEnabled)
{
	m_action_proxies[action] = actionEnabled;
}

void Player::SetMissionStatus(MissionStatus status)
{
	m_current_mission_status = status;
}

MissionStatus Player::GetMissionStatus() const
{
	return m_current_mission_status;
}

void Player::InitialiseActions()
{	
	m_action_binding[PlayerActions::kMoveForwards].action = DerivedAction<Tank>(TankMover(0.0f, -1.0f, m_identifier));
	m_action_binding[PlayerActions::kMoveBackwards].action = DerivedAction<Tank>(TankMover(0.0f, 1.0f, m_identifier));
	m_action_binding[PlayerActions::kRotateRight].action = DerivedAction<Tank>(TankRotator(1.0f, m_identifier));
	m_action_binding[PlayerActions::kRotateLeft].action = DerivedAction<Tank>(TankRotator(-1.0f, m_identifier));
	m_action_binding[PlayerActions::kRotateCannonRight].action = DerivedAction<Tank>(CannonRotator(1.0f, m_identifier));
	m_action_binding[PlayerActions::kRotateCannonLeft].action = DerivedAction<Tank>(CannonRotator(-1.0f, m_identifier));
	m_action_binding[PlayerActions::kFire].action = DerivedAction<Tank>(TankFireTrigger(m_identifier));
	m_action_binding[PlayerActions::kGuidedMissile].action = DerivedAction<Tank>(TankMissileTrigger(m_identifier));
}