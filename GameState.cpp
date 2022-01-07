#include "GameState.hpp"
#include "Player.hpp"

#include <iostream>

GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, m_world(*context.window, *context.fonts)
	, m_player(*context.player)
	, m_player_2(*context.player2)
{
}

void GameState::Draw()
{
	m_world.Draw();
}

bool GameState::Update(sf::Time dt)
{
	m_world.Update(dt);
	if (!m_world.HasAlivePlayer())
	{
		m_player.SetMissionStatus(MissionStatus::kMissionFailure);
		m_player_2.SetMissionStatus(MissionStatus::kMissionFailure);
		RequestStackPush(StateID::kGameOver);
	}
	else if (m_world.HasPlayerReachedEnd())
	{
		m_player.SetMissionStatus(MissionStatus::kMissionSuccess);
		m_player_2.SetMissionStatus(MissionStatus::kMissionSuccess);
		RequestStackPush(StateID::kGameOver);
	}
	CommandQueue& commands = m_world.getCommandQueue();
	m_player.HandleRealtimeInput(commands);
	m_player_2.HandleRealtimeInput(commands);
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = m_world.getCommandQueue();
	m_player.HandleEvent(event, commands);
	m_player_2.HandleEvent(event, commands);

	//Escape or start on the gamepad should bring up the Pause Menu
	if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
	{
		RequestStackPush(StateID::kPause);
	}
	return true;
}