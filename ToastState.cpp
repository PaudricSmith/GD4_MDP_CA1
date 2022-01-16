/////////////////////////
// DkIT
// Paudric Smith
// D00215637
/////////////////////////
// A Class that handles the toast scene just before the game starts
/////////////////////////////////////////////////////////////////////

#include "ToastState.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Utility.hpp"

ToastState::ToastState(StateStack& stack, Context context)
	: State(stack, context)
	, m_world(*context.window, *context.textures, *context.fonts, *context.sounds)
	, m_sfx_player(*context.sounds)
	, m_ready_text()
	, m_steady_text()
	, m_go_text()
	, m_elapsed_time(sf::Time::Zero)
	, m_beep_1_played(false)
	, m_beep_2_played(false)
{

	sf::Font& font = context.fonts->Get(Fonts::Main);
	sf::Vector2f windowSize(context.window->getSize());

	m_ready_text.setFont(font);
	m_ready_text.setString("Ready!");
	m_ready_text.setCharacterSize(70);
	Utility::CentreOrigin(m_ready_text);
	m_ready_text.setPosition(0.5f * windowSize.x, 0.4f * windowSize.y);

	m_steady_text.setFont(font);
	m_steady_text.setString("Steady!");
	m_steady_text.setCharacterSize(90);
	Utility::CentreOrigin(m_steady_text);
	m_steady_text.setPosition(0.5f * windowSize.x, 0.5f * windowSize.y);

	// Stop currently playing music
	context.musicPlayer->Stop();

	// Stop currently playing SFX
	m_sfx_player.RemovePlayingSounds();
}

/// <summary>
/// Draws the game screen with a dark transparent rectange shape over it 
/// to see the toast better and know the game didn't start yet.
/// Draws the toast word depending on a boolean
/// </summary>
void ToastState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());

	m_world.Draw();

	// Create dark, semitransparent background
	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);

	if (m_beep_2_played)
	{
		window.draw(m_steady_text);
	}
	if (m_beep_1_played)
	{
		window.draw(m_ready_text);
	}
}

/// <summary>
/// Updates a timer to trigger booleans 
/// and play a sound each second then change state to the game
/// </summary>
/// <param name="dt"></param>
/// <returns></returns>
bool ToastState::Update(sf::Time dt)
{
	// Show state for 3 seconds and screen toast, after, start game
	m_elapsed_time += dt;
	
	if (m_elapsed_time > sf::seconds(3))
	{
		RequestStackPop();
		RequestStackPush(StateID::kGame);
	}
	else if (m_elapsed_time > sf::seconds(2) && m_beep_2_played == false)
	{
		// Play beep SFX
		m_sfx_player.Play(SoundEffects::kToastBeep1);

		m_beep_2_played = true;
	}
	else if (m_elapsed_time > sf::seconds(1) && m_beep_1_played == false)
	{
		// Play beep SFX
		m_sfx_player.Play(SoundEffects::kToastBeep1);

		m_beep_1_played = true;
	}
	
	return false;
}

bool ToastState::HandleEvent(const sf::Event&)
{
	return false;
}