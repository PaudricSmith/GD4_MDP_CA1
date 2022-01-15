#include "GameOverState.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Player.hpp"
#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include <iostream>

GameOverState::GameOverState(StateStack& stack, Context context)
	: State(stack, context)
	, m_game_over_text()
	, m_elapsed_time(sf::Time::Zero)
{
	sf::Font& font = context.fonts->Get(Fonts::Main);
	sf::Vector2f windowSize(context.window->getSize());

	m_game_over_text.setFont(font);
	if (context.player->GetMissionStatus() == MissionStatus::kMissionSuccess)
		m_game_over_text.setString("Player 1 Camo Tank Wins!");
	else if (context.player->GetMissionStatus() == MissionStatus::kMissionFailure)
		m_game_over_text.setString("Player 2 Sand Tank Wins!");
	else if (context.player2->GetMissionStatus() == MissionStatus::kMissionSuccess)
		m_game_over_text.setString("Player 2 Sand Tank Wins!");
	else if (context.player2->GetMissionStatus() == MissionStatus::kMissionFailure)
		m_game_over_text.setString("Player 1 Camo Tank Wins!");

	m_game_over_text.setCharacterSize(70);
	Utility::CentreOrigin(m_game_over_text);
	m_game_over_text.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);

	context.musicPlayer->Stop();
	context.sounds->Play(SoundEffects::kWinJingle);
}

void GameOverState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());

	// Create dark, semitransparent background
	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(m_game_over_text);
}

bool GameOverState::Update(sf::Time dt)
{
	// Show state for 5 seconds, after return to menu
	m_elapsed_time += dt;
	if (m_elapsed_time > sf::seconds(5))
	{
		RequestStackClear();
		RequestStackPush(StateID::kMenu);
	}
	return false;
}

bool GameOverState::HandleEvent(const sf::Event&)
{
	return false;
}