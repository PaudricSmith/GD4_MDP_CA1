#include "Player.hpp"
#include "Tank.hpp"
#include <algorithm>
#include <iostream>

struct TankMover
{
	TankMover(float vx, float vy) : velocity(vx, vy)
	{}

	void operator()(Tank& tank, sf::Time) const
	{
		tank.Accelerate(velocity * tank.GetMaxSpeed());
	}

	sf::Vector2f velocity;
};

struct TankRotator
{
	TankRotator(float angle) : rotation(angle)
	{}

	void operator()(Tank& tank, sf::Time) const
	{
		tank.Rotate(rotation * tank.GetRotationSpeed());
	}

	float rotation;
};


Player::Player() : m_current_mission_status(MissionStatus::kMissionRunning)
{

	//Set initial key bindings
	m_key_binding[sf::Keyboard::A] = PlayerAction::kMoveLeft;
	m_key_binding[sf::Keyboard::Left] = PlayerAction::kMoveLeft;
	//m_joystick_binding[sf::Joystick::Left] = PlayerAction::kMoveLeft;

	m_key_binding[sf::Keyboard::D] = PlayerAction::kMoveRight;
	m_key_binding[sf::Keyboard::Right] = PlayerAction::kMoveRight;
	//m_key_binding[sf::Joystick::Right] = PlayerAction::kMoveRight;

	m_key_binding[sf::Keyboard::W] = PlayerAction::kMoveUp;
	m_key_binding[sf::Keyboard::Up] = PlayerAction::kMoveUp;
	//m_key_binding[sf::Joystick::Up] = PlayerAction::kMoveUp;

	m_key_binding[sf::Keyboard::S] = PlayerAction::kMoveDown;
	m_key_binding[sf::Keyboard::Down] = PlayerAction::kMoveDown;
	//m_key_binding[sf::Joystick::Down] = PlayerAction::kMoveDown;

	m_key_binding[sf::Keyboard::Q] = PlayerAction::kRotateLeft;

	m_key_binding[sf::Keyboard::E] = PlayerAction::kRotateRight;

	m_key_binding[sf::Keyboard::Space] = PlayerAction::kFire;
	//m_key_binding[sf::Joystick::Space] = PlayerAction::kFire;

	m_key_binding[sf::Keyboard::M] = PlayerAction::kLaunchMissile;
	//m_key_binding[sf::Joystick::M] = PlayerAction::kLaunchMissile;

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
	const float player_speed = 200.f;
	const float speed_multiplier = 0.5f;

	m_action_binding[PlayerAction::kMoveLeft].action = DerivedAction<Tank>(TankMover(-1 * speed_multiplier, 0.f));
	m_action_binding[PlayerAction::kMoveRight].action = DerivedAction<Tank>(TankMover(+1 * speed_multiplier, 0.f));
	m_action_binding[PlayerAction::kMoveUp].action = DerivedAction<Tank>(TankMover(0.f, -1 * speed_multiplier));
	m_action_binding[PlayerAction::kMoveDown].action = DerivedAction<Tank>(TankMover(0, 1 * speed_multiplier));
	
	m_action_binding[PlayerAction::kRotateLeft].action = DerivedAction<Tank>(TankRotator(-1.f));
	m_action_binding[PlayerAction::kRotateRight].action = DerivedAction<Tank>(TankRotator(1.f));

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
	case PlayerAction::kFire:
		return true;
	default:
		return false;
	}
}