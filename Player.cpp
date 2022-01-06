#include "Player.hpp"
#include "Tank.hpp"
#include "Utility.hpp"

#include <algorithm>
#include <iostream>

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
		std::cout << "\nTank Rotation Angle == " << tank.getRotation();
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


Player::Player() : m_current_mission_status(MissionStatus::kMissionRunning)
{
	//Set initial key bindings
	
	// Move Forwards 
	m_key_binding[sf::Keyboard::W] = PlayerAction::kMoveUp;
	m_key_binding[sf::Keyboard::Up] = PlayerAction::kMoveUp;

	// Move Backwards
	m_key_binding[sf::Keyboard::S] = PlayerAction::kMoveDown;
	m_key_binding[sf::Keyboard::Down] = PlayerAction::kMoveDown;

	// Rotation Of Tank
	m_key_binding[sf::Keyboard::A] = PlayerAction::kRotateLeft;
	m_key_binding[sf::Keyboard::D] = PlayerAction::kRotateRight;

	// Rotation Of Cannon
	m_key_binding[sf::Keyboard::Q] = PlayerAction::kRotateCannonLeft;
	m_key_binding[sf::Keyboard::E] = PlayerAction::kRotateCannonRight;

	// Weapons
	m_key_binding[sf::Keyboard::Space] = PlayerAction::kFire;
	m_key_binding[sf::Keyboard::M] = PlayerAction::kLaunchMissile;


	//Set initial action bindings
	InitialiseActions();

	//Assign all categories to the player's tank
	for (auto& pair : m_action_binding)
	{
		pair.second.category = Category::kPlayerTank;
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
	//std::cout << "In Realtime Input when pressed Left movement key!" << std::endl;

	//Check if any keybinding keys are pressed
	for (auto pair : m_key_binding)
	{
		if (sf::Keyboard::isKeyPressed(pair.first) && IsRealtimeAction(pair.second))
		{
			/*std::cout << "In Realtime Input when pressed Left movement key!" << std::endl;*/

			commands.Push(m_action_binding[pair.second]);
		}
	}

	// JOYSTICK MOVEMENT CONTROLS
	/*if (sf::Joystick::PovX)
	{
		if (event.joystickMove.axis == sf::Joystick::X)
		{
			if (event.joystickMove.position < 0 && !IsRealtimeAction(PlayerAction::kMoveLeft))
			{
				std::cout << "Joystick Id: " << event.joystickMove.joystickId << std::endl;
				std::cout << "X axis Left!" << std::endl;
				std::cout << "New position: " << event.joystickMove.position << std::endl;

				commands.Push(m_action_binding[PlayerAction::kMoveLeft]);
			}
		}
	}*/

	// JOYSTICK MOVEMENT CONTROLS
	/*if (event.type == sf::Event::JoystickMoved)
	{
		std::cout << "New position: " << event.joystickMove.position << std::endl;
		if (event.joystickMove.axis == sf::Joystick::X)
		{
			if (event.joystickMove.position < -1.0f && IsRealtimeAction(PlayerAction::kMoveLeft))
			{
				std::cout << "Joystick Id: " << event.joystickMove.joystickId << std::endl;
				std::cout << "X axis Left!" << std::endl;
				std::cout << "New position: " << event.joystickMove.position << std::endl;

				commands.Push(m_action_binding[PlayerAction::kMoveLeft]);
			}
		}
	}*/
}

void Player::AssignKey(PlayerAction action, sf::Keyboard::Key key)
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

sf::Keyboard::Key Player::GetAssignedKey(PlayerAction action) const
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
	//const float player_speed = 200.f;
	const float speed_multiplier = 0.5f;

	/*m_action_binding[PlayerAction::kMoveLeft].action = DerivedAction<Tank>(TankMover(-1 * speed_multiplier, 0.f));
	m_action_binding[PlayerAction::kMoveRight].action = DerivedAction<Tank>(TankMover(+1 * speed_multiplier, 0.f));*/

	m_action_binding[PlayerAction::kMoveUp].action = DerivedAction<Tank>(TankMover(0.f, -1 * speed_multiplier));
	m_action_binding[PlayerAction::kMoveDown].action = DerivedAction<Tank>(TankMover(0, 1 * speed_multiplier));
	
	m_action_binding[PlayerAction::kRotateLeft].action = DerivedAction<Tank>(TankRotator(-1.f));
	m_action_binding[PlayerAction::kRotateRight].action = DerivedAction<Tank>(TankRotator(1.f));

	m_action_binding[PlayerAction::kRotateCannonLeft].action = DerivedAction<Tank>(CannonRotator(-1.f));
	m_action_binding[PlayerAction::kRotateCannonRight].action = DerivedAction<Tank>(CannonRotator(1.f));

	m_action_binding[PlayerAction::kFire].action = DerivedAction<Tank>([](Tank& a, sf::Time
		)
		{
			a.Fire();
		});

	m_action_binding[PlayerAction::kLaunchMissile].action = DerivedAction<Tank>([](Tank& a, sf::Time
		)
		{
			a.LaunchMissile();
		});
}

bool Player::IsRealtimeAction(PlayerAction action)
{
	switch (action)
	{
	case PlayerAction::kMoveLeft:
	case PlayerAction::kMoveRight:
	case PlayerAction::kMoveUp:
	case PlayerAction::kMoveDown:
	case PlayerAction::kRotateLeft:
	case PlayerAction::kRotateRight:
	case PlayerAction::kRotateCannonLeft:
	case PlayerAction::kRotateCannonRight:
	case PlayerAction::kFire:
		return true;
	default:
		return false;
	}
}