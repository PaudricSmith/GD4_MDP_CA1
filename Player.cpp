#include <algorithm>
#include <iostream>

#include "Player.hpp"
#include "Tank.hpp"
#include "Utility.hpp"
#include "DataTables.hpp"


namespace
{
	const std::vector<PlayerData> Table = InitializePlayerData();
}

struct TankMover
{
	TankMover(float vx, float vy) : velocity(vx, vy)
	{}

	void operator()(Tank& tank, sf::Time) const
	{
		// Movement in direction of forward code based on this webpage
	    // https://www.cplusplus.com/forum/general/110659/ 
		
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
	}

	sf::Vector2f velocity;
};

struct TankRotator // Rotate Tank
{
	TankRotator(float angle) : rotation(angle)
	{}

	void operator()(Tank& tank, sf::Time) const
	{
		tank.Rotate(rotation * tank.GetRotationSpeed()); // Rotate Tank by sign and Tank rotation speed
	}

	float rotation;
};

struct CannonRotator // Rotate Cannon
{
	CannonRotator(float angle) : rotation(angle)
	{}

	void operator()(Tank& tank, sf::Time) const
	{
		tank.RotateCannon(rotation * tank.GetCannonRotationSpeed()); // Rotate Tank Cannon by sign and cannon speed
	}

	float rotation;
};

Player::Player(PlayerNumber playerNumber) : m_current_mission_status(MissionStatus::kMissionRunning)
	, m_player_number(playerNumber)
{
	
	//Set initial key bindings

	if (m_player_number == PlayerNumber::kPlayer1) // Player 1 Initial Key bindings
	{
		// Move Forwards 
		m_key_binding[sf::Keyboard::W] = PlayerActions::kMoveForwards;

		// Move Backwards
		m_key_binding[sf::Keyboard::S] = PlayerActions::kMoveBackwards;

		// Rotation Of Tank
		m_key_binding[sf::Keyboard::A] = PlayerActions::kRotateLeft;
		m_key_binding[sf::Keyboard::D] = PlayerActions::kRotateRight;

		// Rotation Of Cannon
		m_key_binding[sf::Keyboard::Q] = PlayerActions::kRotateCannonLeft;
		m_key_binding[sf::Keyboard::E] = PlayerActions::kRotateCannonRight;

		// Weapons
		m_key_binding[sf::Keyboard::LShift] = PlayerActions::kFire;
		m_key_binding[sf::Keyboard::LControl] = PlayerActions::kLaunchMissile;

	}
	else if (m_player_number == PlayerNumber::kPlayer2) // Player 2 Initial Key Bindings
	{
		// Move Forwards 
		m_key_binding[sf::Keyboard::I] = PlayerActions::kMoveForwards2;

		// Move Backwards
		m_key_binding[sf::Keyboard::K] = PlayerActions::kMoveBackwards2;

		// Rotation Of Tank
		m_key_binding[sf::Keyboard::J] = PlayerActions::kRotateLeft2;
		m_key_binding[sf::Keyboard::L] = PlayerActions::kRotateRight2;

		// Rotation Of Cannon
		m_key_binding[sf::Keyboard::U] = PlayerActions::kRotateCannonLeft2;
		m_key_binding[sf::Keyboard::O] = PlayerActions::kRotateCannonRight2;

		// Weapons
		m_key_binding[sf::Keyboard::Space] = PlayerActions::kFire2;
		m_key_binding[sf::Keyboard::Slash] = PlayerActions::kLaunchMissile2;
	}
	

	//Set initial action bindings
	InitialiseActions();


	//Assign player category to the player 1 
	for (auto& pair : m_action_binding)
	{
		pair.second.category = static_cast<int>(Table[static_cast<int>(m_player_number)].playerCategory);
	}

}


void Player::HandleEvent(const sf::Event& event, CommandQueue& commands)
{
	if (event.type == sf::Event::KeyPressed)
	{
		auto found = m_key_binding.find(event.key.code);
		if (found != m_key_binding.end() && !IsRealtimeAction(found->second))
		{
			commands.Push(m_action_binding[found->second]);
		}
	}

	
	else if (event.type == sf::Event::JoystickButtonPressed)
	{
		std::cout << "joystick 'A' button pressed: " << event.joystickButton.button << std::endl;

		if (event.joystickButton.button == 0)
		std::cout << "joystick 'A' button pressed: " << event.joystickButton.button << std::endl;
	}
	else if (event.type == sf::Event::JoystickConnected)
	{
		std::cout << "joystick connected: " << event.joystickConnect.joystickId << std::endl;
	}
	else if (event.type == sf::Event::JoystickDisconnected)
	{
		std::cout << "joystick disconnected: " << event.joystickConnect.joystickId << std::endl;
	}
}

void Player::HandleRealtimeInput(CommandQueue& commands)
{
	//Check if any keybinding keys are pressed
	for (auto pair : m_key_binding)
	{
		if (sf::Keyboard::isKeyPressed(pair.first) && IsRealtimeAction(pair.second))
		{
			commands.Push(m_action_binding[pair.second]);
		}
	}
}

void Player::AssignKey(PlayerActions action, sf::Keyboard::Key key)
{
	//Remove all keys that are already bound to action
	for (auto itr = m_key_binding.begin(); itr != m_key_binding.end();)
	{
		if (itr->second == action)
		{
			m_key_binding.erase(itr++);
		}
		else
		{
			++itr;
		}
	}
	m_key_binding[key] = action;
}

sf::Keyboard::Key Player::GetAssignedKey(PlayerActions action) const
{
	for (auto pair : m_key_binding)
	{
		if (pair.second == action)
		{
			return pair.first;
		}
	}
	return sf::Keyboard::Unknown;
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
	const float speed_multiplier = 0.5f;

	// Player 1 Action Bindings
	m_action_binding[PlayerActions::kMoveForwards].action = DerivedAction<Tank>(TankMover(0.f, -1 * speed_multiplier));
	m_action_binding[PlayerActions::kMoveBackwards].action = DerivedAction<Tank>(TankMover(0, 1 * speed_multiplier));
	
	m_action_binding[PlayerActions::kRotateLeft].action = DerivedAction<Tank>(TankRotator(-1.f));
	m_action_binding[PlayerActions::kRotateRight].action = DerivedAction<Tank>(TankRotator(1.f));

	m_action_binding[PlayerActions::kRotateCannonLeft].action = DerivedAction<Tank>(CannonRotator(-1.f));
	m_action_binding[PlayerActions::kRotateCannonRight].action = DerivedAction<Tank>(CannonRotator(1.f));

	m_action_binding[PlayerActions::kFire].action = DerivedAction<Tank>([](Tank& a, sf::Time){a.Fire();});
	m_action_binding[PlayerActions::kLaunchMissile].action = DerivedAction<Tank>([](Tank& a, sf::Time){a.LaunchMissile();});


	// Player 2 Action Bindings
	m_action_binding[PlayerActions::kMoveForwards2].action = DerivedAction<Tank>(TankMover(0.f, -1 * speed_multiplier));
	m_action_binding[PlayerActions::kMoveBackwards2].action = DerivedAction<Tank>(TankMover(0, 1 * speed_multiplier));

	m_action_binding[PlayerActions::kRotateLeft2].action = DerivedAction<Tank>(TankRotator(-1.f));
	m_action_binding[PlayerActions::kRotateRight2].action = DerivedAction<Tank>(TankRotator(1.f));

	m_action_binding[PlayerActions::kRotateCannonLeft2].action = DerivedAction<Tank>(CannonRotator(-1.f));
	m_action_binding[PlayerActions::kRotateCannonRight2].action = DerivedAction<Tank>(CannonRotator(1.f));

	m_action_binding[PlayerActions::kFire2].action = DerivedAction<Tank>([](Tank& a, sf::Time) {a.Fire(); });
	m_action_binding[PlayerActions::kLaunchMissile2].action = DerivedAction<Tank>([](Tank& a, sf::Time) {a.LaunchMissile(); });

}

bool Player::IsRealtimeAction(PlayerActions action)
{
	switch (action)
	{
	case PlayerActions::kMoveForwards:
	case PlayerActions::kMoveBackwards:
	case PlayerActions::kRotateLeft:
	case PlayerActions::kRotateRight:
	case PlayerActions::kRotateCannonLeft:
	case PlayerActions::kRotateCannonRight:
	case PlayerActions::kFire:
	case PlayerActions::kMoveForwards2:
	case PlayerActions::kMoveBackwards2:
	case PlayerActions::kRotateLeft2:
	case PlayerActions::kRotateRight2:
	case PlayerActions::kRotateCannonLeft2:
	case PlayerActions::kRotateCannonRight2:
	case PlayerActions::kFire2:
		return true;
	default:
		return false;
	}
}