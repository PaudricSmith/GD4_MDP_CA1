#include "KeyBinding.hpp"

#include <string>
#include <algorithm>


KeyBinding::KeyBinding(int control_preconfiguration)
	: m_key_map()
{
	// Set initial key bindings for player 1
	if (control_preconfiguration == 1)
	{
		m_key_map[sf::Keyboard::A] = PlayerActions::kRotateLeft;
		m_key_map[sf::Keyboard::D] = PlayerActions::kRotateRight;
		m_key_map[sf::Keyboard::Q] = PlayerActions::kRotateCannonLeft;
		m_key_map[sf::Keyboard::E] = PlayerActions::kRotateCannonRight;
		m_key_map[sf::Keyboard::W] = PlayerActions::kMoveForwards;
		m_key_map[sf::Keyboard::S] = PlayerActions::kMoveBackwards;
		m_key_map[sf::Keyboard::LShift] = PlayerActions::kFire;
		m_key_map[sf::Keyboard::LControl] = PlayerActions::kGuidedMissile;

	}
	else if (control_preconfiguration == 2)
	{
		// Player 2
		m_key_map[sf::Keyboard::J] = PlayerActions::kRotateLeft;
		m_key_map[sf::Keyboard::L] = PlayerActions::kRotateRight;
		m_key_map[sf::Keyboard::U] = PlayerActions::kRotateCannonLeft;
		m_key_map[sf::Keyboard::O] = PlayerActions::kRotateCannonRight;
		m_key_map[sf::Keyboard::I] = PlayerActions::kMoveForwards;
		m_key_map[sf::Keyboard::K] = PlayerActions::kMoveBackwards;
		m_key_map[sf::Keyboard::Space] = PlayerActions::kFire;
		m_key_map[sf::Keyboard::M] = PlayerActions::kGuidedMissile;
	}
}

void KeyBinding::AssignKey(Action action, sf::Keyboard::Key key)
{
	// Remove all keys that already map to action
	for (auto itr = m_key_map.begin(); itr != m_key_map.end(); )
	{
		if (itr->second == action)
			m_key_map.erase(itr++);
		else
			++itr;
	}

	// Insert new binding
	m_key_map[key] = action;
}

sf::Keyboard::Key KeyBinding::GetAssignedKey(Action action) const
{
	for (auto pair : m_key_map)
	{
		if (pair.second == action)
			return pair.first;
	}

	return sf::Keyboard::Unknown;
}

bool KeyBinding::CheckAction(sf::Keyboard::Key key, Action& out) const
{
	auto found = m_key_map.find(key);
	if (found == m_key_map.end())
	{
		return false;
	}
	else
	{
		out = found->second;
		return true;
	}
}

std::vector<KeyBinding::Action> KeyBinding::GetRealtimeActions() const
{
	// Return all realtime actions that are currently active.
	std::vector<Action> actions;

	for (auto pair : m_key_map)
	{
		// If key is pressed and an action is a realtime action, store it
		if (sf::Keyboard::isKeyPressed(pair.first) && IsRealtimeAction(pair.second))
			actions.push_back(pair.second);
	}

	return actions;
}

bool IsRealtimeAction(PlayerActions action)
{
	switch (action)
	{
	case PlayerActions::kMoveForwards:
	case PlayerActions::kMoveBackwards:
	case PlayerActions::kRotateRight:
	case PlayerActions::kRotateLeft:
	case PlayerActions::kRotateCannonRight:
	case PlayerActions::kRotateCannonLeft:
	case PlayerActions::kFire:
		return true;

	default:
		return false;
	}
}