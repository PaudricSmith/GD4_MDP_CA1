#include "PauseState.hpp"
#include "ResourceHolder.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>

#include "Button.hpp"
#include "Utility.hpp"



PauseState::PauseState(StateStack& stack, Context context, bool let_updates_through)
	: State(stack, context)
	, m_let_updates_through(let_updates_through)
	, m_background_sprite()
	, m_paused_text()
	, m_instruction_text()
{
	sf::Font& font = context.fonts->Get(Fonts::Main);
	sf::Vector2f viewSize = context.window->getView().getSize();

	m_paused_text.setFont(font);
	m_paused_text.setString("Game Paused");
	m_paused_text.setCharacterSize(70);
	Utility::CentreOrigin(m_paused_text);
	m_paused_text.setPosition(0.5f * viewSize.x, 0.4f * viewSize.y);

	auto returnButton = std::make_shared<GUI::Button>(context);
	returnButton->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 75);
	returnButton->SetText("Return");
	returnButton->SetCallback([this]()
		{
			RequestStackPop();
		});
	auto backToMenuButton = std::make_shared<GUI::Button>(context);
	backToMenuButton->setPosition(0.5f * viewSize.x - 100, 0.4f * viewSize.y + 125);
	backToMenuButton->SetText("Back to menu");
	backToMenuButton->SetCallback([this]()
		{
			RequestStackClear();
			RequestStackPush(StateID::kMenu);
		});
	m_gui_container.Pack(returnButton);
	m_gui_container.Pack(backToMenuButton);

	m_instruction_text.setFont(font);
	m_instruction_text.setString("(Press Backspace to return to the main menu)");
	Utility::CentreOrigin(m_instruction_text);
	m_instruction_text.setPosition(0.5f * viewSize.x, 0.6f * viewSize.y);

	// Pause in-game music
	context.musicPlayer->SetPaused(true);
}

PauseState::~PauseState()
{
	GetContext().musicPlayer->SetPaused(false);
}

void PauseState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());

	sf::RectangleShape backgroundShape;
	backgroundShape.setFillColor(sf::Color(0, 0, 0, 150));
	backgroundShape.setSize(window.getView().getSize());

	window.draw(backgroundShape);
	window.draw(m_paused_text);
	window.draw(m_instruction_text);
	window.draw(m_gui_container);
}

bool PauseState::Update(sf::Time)
{
	return m_let_updates_through;
}

bool PauseState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);

	return false;
}