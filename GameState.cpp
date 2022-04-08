#include "GameState.hpp"
#include <iostream>
#include "Player.hpp"


GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, m_world(*context.window, *context.textures, *context.fonts, *context.sounds, false)
	, m_player(nullptr, 1, context.keys1)
{
	m_world.AddTank(1);
	
	std::cout << "New Tank = " << m_world.GetTank(1) << std::endl;

	m_player.SetMissionStatus(MissionStatus::kMissionRunning);

	// Play Menu Music Track
	context.musicPlayer->Play(MusicTracks::kLevel1Track);
}

void GameState::Draw()
{
	m_world.Draw();
}

bool GameState::Update(sf::Time dt)
{

	m_world.Update(dt);

	if (!m_world.HasAlivePlayer1())
	{
		m_player.SetMissionStatus(MissionStatus::kMissionFailure);
		RequestStackPush(StateID::kGameOver);
	}
	
	//if (!m_world.HasAlivePlayer1()) // If Player 1 has died in the world
	//{
	//	m_player.SetMissionStatus(MissionStatus::kMissionFailure);
	//	
	//	RequestStackPush(StateID::kGameOver);
	//}
	//else if (!m_world.HasAlivePlayer2()) // If Player 2 has died in the world
	//{
	//	m_player.SetMissionStatus(MissionStatus::kMissionSuccess);
	//
	//	RequestStackPush(StateID::kGameOver);
	//}

	CommandQueue& commands = m_world.GetCommandQueue();
	m_player.HandleRealtimeInput(commands);
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = m_world.GetCommandQueue();
	m_player.HandleEvent(event, commands);

	//Escape or start on the gamepad should bring up the Pause Menu
	if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
	{
		RequestStackPush(StateID::kPause);
	}
	return true;
}