#pragma once

#include <map>
#include <SFML/Window/Event.hpp>

#include "Command.hpp"
#include "KeyBinding.hpp"
#include <SFML/Network/TcpSocket.hpp>
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
	Player(sf::TcpSocket* socket, sf::Int32 identifier, const KeyBinding* binding);
	void HandleEvent(const sf::Event& event, CommandQueue& commands);
	void HandleRealtimeInput(CommandQueue& commands);
	void HandleRealtimeNetworkInput(CommandQueue& commands);

	/*void AssignKey(PlayerActions action, sf::Keyboard::Key key);
	sf::Keyboard::Key GetAssignedKey(PlayerActions action) const;*/

	//React to events or realtime state changes recevied over the network
	void HandleNetworkEvent(PlayerActions action, CommandQueue& commands);
	void HandleNetworkRealtimeChange(PlayerActions action, bool action_enabled);

	void SetMissionStatus(MissionStatus status);
	MissionStatus GetMissionStatus() const;
	//int GetPlayerNumber() const;

	void DisableAllRealtimeActions();
	bool IsLocal() const;

private:
	void InitialiseActions();
	//static bool IsRealtimeAction(PlayerActions action);

private:
	//PlayerNumber m_player_number;
	const KeyBinding* m_key_binding;

	//std::map<sf::Keyboard::Key, PlayerActions> m_key_binding;
	std::map<PlayerActions, Command> m_action_binding;
	std::map<PlayerActions, bool> m_action_proxies;
	MissionStatus m_current_mission_status;
	int m_identifier;
	sf::TcpSocket* m_socket;
};