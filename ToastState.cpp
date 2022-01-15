#include "ToastState.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "Utility.hpp"

ToastState::ToastState(StateStack& stack, Context context)
	: State(stack, context)
	, m_world(*context.window, *context.textures, *context.fonts, *context.sounds)
	, m_ready_text()
	, m_steady_text()
	, m_go_text()
	, m_elapsed_time(sf::Time::Zero)
	, m_draw_text_1(false)
	, m_draw_text_2(false)
	, m_draw_text_3(false)
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
	m_steady_text.setCharacterSize(70);
	Utility::CentreOrigin(m_steady_text);
	m_steady_text.setPosition(0.5f * windowSize.x, 0.5f * windowSize.y);

	m_go_text.setFont(font);
	m_go_text.setString("Go!");
	m_go_text.setCharacterSize(70);
	Utility::CentreOrigin(m_go_text);
	m_go_text.setPosition(0.5f * windowSize.x, 0.6f * windowSize.y);


	// Stop currently playing music
	context.musicPlayer->Stop();
	
}

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

	if (m_draw_text_1 && m_draw_text_2 && m_draw_text_3)
	{
		window.draw(m_go_text);
	}
	else if (m_draw_text_1 && m_draw_text_2)
	{
		window.draw(m_steady_text);
	}
	else if (m_draw_text_1)
	{
		window.draw(m_ready_text);
	}
    
}

bool ToastState::Update(sf::Time dt)
{

	// Show state for 4 seconds and screen toast, after return to game
	m_elapsed_time += dt;
	
	if (m_elapsed_time > sf::seconds(4))
	{
		RequestStackPop();
		RequestStackPush(StateID::kGame);
	}
	else if (m_elapsed_time > sf::seconds(3))
	{
		m_draw_text_3 = true;
	}
	else if (m_elapsed_time > sf::seconds(2))
	{
		m_draw_text_2 = true;
	}
	else if (m_elapsed_time > sf::seconds(1))
	{
		m_draw_text_1 = true;
	}
	
	return false;
}

bool ToastState::HandleEvent(const sf::Event&)
{
	return false;
}