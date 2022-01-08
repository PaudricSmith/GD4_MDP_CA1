#pragma once

#include <map>
#include <SFML/Window/Event.hpp>

#include "Command.hpp"
#include "CommandQueue.hpp"
#include "MissionStatus.hpp"
#include "PlayerActions.hpp"
#include "PlayerNumber.hpp"


enum class PlayerNumber
{
	kPlayer1,
	kPlayer2,
	kPlayerCount
};


class Player
{
public:
	Player(PlayerNumber playerNumber);
	void HandleEvent(const sf::Event& event, CommandQueue& commands);
	void HandleRealtimeInput(CommandQueue& commands);

	void AssignKey(PlayerActions action, sf::Keyboard::Key key);
	sf::Keyboard::Key GetAssignedKey(PlayerActions action) const;
	void SetMissionStatus(MissionStatus status);
	MissionStatus GetMissionStatus() const;

private:
	void InitialiseActions();
	static bool IsRealtimeAction(PlayerActions action);

private:
	PlayerNumber m_player_number;

	std::map<sf::Keyboard::Key, PlayerActions> m_key_binding;
	std::map<PlayerActions, Command> m_action_binding;

	MissionStatus m_current_mission_status;
};