#include "GameState.hpp"
#include <iostream>
#include "Player.hpp"


GameState::GameState(StateStack& stack, Context context)
	: State(stack, context)
	, m_world(*context.window, *context.textures, *context.fonts, *context.sounds, false)
	, m_player(nullptr, 1, context.keys1)
	, m_player_2(nullptr, 2, context.keys2)
{
	m_world.AddTank(1);
	m_world.AddTank(2);
	
	std::cout << "New Tank 1 = " << m_world.GetTank(1) << std::endl;
	std::cout << "New Tank 2 = " << m_world.GetTank(2) << std::endl;
	//std::cout << "New Tank 3 = " << m_world.GetTank(3) << std::endl;

	//m_player.SetMissionStatus(MissionStatus::kMissionRunning);
	//m_player_2.SetMissionStatus(MissionStatus::kMissionRunning);

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
	
	if (!m_world.GetTank(1)) // If Player 1 has died in the world
	{		
		RequestStackPush(StateID::kSuccessYellow);
	}
	else if (!m_world.GetTank(2)) // If Player 2 has died in the world
	{	
		RequestStackPush(StateID::kSuccessGreen);
	}

	CommandQueue& commands = m_world.GetCommandQueue();
	m_player.HandleRealtimeInput(commands);
	m_player_2.HandleRealtimeInput(commands);
	return true;
}

bool GameState::HandleEvent(const sf::Event& event)
{
	CommandQueue& commands = m_world.GetCommandQueue();
	m_player.HandleEvent(event, commands);
	m_player_2.HandleEvent(event, commands);

	//Escape or start on the gamepad should bring up the Pause Menu
	if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Escape)
	{
		RequestStackPush(StateID::kPause);
	}
	return true;
}