#include "SettingsState.hpp"
#include "Utility.hpp"
#include "ResourceHolder.hpp"
#include "StateStack.hpp"

#include <SFML/Graphics/RenderWindow.hpp>


SettingsState::SettingsState(StateStack& stack, Context context)
	: State(stack, context)
	, m_gui_container()
{
	m_playerActionsHalfSize = static_cast<int>(PlayerActions::kActionCount) / 2;

	m_background_sprite.setTexture(context.textures->Get(Textures::kKeyBingingsScreen));

	// Build key binding buttons and labels Player 1
	AddP1ButtonLabel(PlayerActions::kMoveForwards, 200.0f, 160.f, 430.0f, "Move Forwards", context);
	AddP1ButtonLabel(PlayerActions::kMoveBackwards, 200.0f, 210.f, 430.0f, "Move Backwards", context);
	AddP1ButtonLabel(PlayerActions::kRotateLeft, 200.0f, 260.f, 430.0f, "Rotate Left", context);
	AddP1ButtonLabel(PlayerActions::kRotateRight, 200.0f, 310.f, 430.0f, "Rotate Right", context);
	AddP1ButtonLabel(PlayerActions::kRotateCannonLeft, 200.0f, 360.f, 430.0f, "Rotate Gun Left", context);
	AddP1ButtonLabel(PlayerActions::kRotateCannonRight, 200.0f, 410.f, 430.0f, "Rotate Gun Right", context);
	AddP1ButtonLabel(PlayerActions::kFire, 200.0f, 460.f, 430.0f, "Fire", context);
	AddP1ButtonLabel(PlayerActions::kGuidedMissile, 200.0f, 510.f, 430.0f, "Missile", context);

	// Build key binding buttons and labels Player 2
	AddP2ButtonLabel(PlayerActions::kMoveForwards2, 600.0f, 160.f, 530.0f, "Move Forwards", context);
	AddP2ButtonLabel(PlayerActions::kMoveBackwards2, 600.0f, 210.f, 530.0f, "Move Backwards", context);
	AddP2ButtonLabel(PlayerActions::kRotateLeft2, 600.0f, 260.f, 530.0f, "Rotate Left", context);
	AddP2ButtonLabel(PlayerActions::kRotateRight2, 600.0f, 310.f, 530.0f, "Rotate Right", context);
	AddP2ButtonLabel(PlayerActions::kRotateCannonLeft2, 600.0f, 360.f, 530.0f, "Rotate Gun Left", context);
	AddP2ButtonLabel(PlayerActions::kRotateCannonRight2, 600.0f, 410.f, 530.0f, "Rotate Gun Right", context);
	AddP2ButtonLabel(PlayerActions::kFire2, 600.0f, 460.f, 530.0f, "Fire", context);
	AddP2ButtonLabel(PlayerActions::kGuidedMissile2, 600.0f, 510.f, 530.0f, "Missile", context);

	UpdateP1Labels();
	UpdateP2Labels();

	auto back_button = std::make_shared<GUI::Button>(context);
	back_button->setPosition(400.f, 620.f);
	back_button->SetText("Back");
	back_button->SetColour(sf::Color::Red);
	back_button->SetCallback(std::bind(&SettingsState::RequestStackPop, this));

	m_gui_container.Pack(back_button);
}

void SettingsState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;

	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool SettingsState::Update(sf::Time)
{
	return true;
}

bool SettingsState::HandleEvent(const sf::Event& event)
{
	bool isKeyBindingP1 = false;
	bool isKeyBindingP2 = false;

	// Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
	for (std::size_t action = 0; action < m_playerActionsHalfSize; ++action)
	{
		if (m_binding_buttons[action]->IsActive())
		{
			isKeyBindingP1 = true;
			if (event.type == sf::Event::KeyReleased)
			{
				GetContext().player->AssignKey(static_cast<PlayerActions>(action), event.key.code);
				m_binding_buttons[action]->Deactivate();
			}
			break;
		}
	}

	// Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
	for (std::size_t action = m_playerActionsHalfSize; action < static_cast<int>(PlayerActions::kActionCount); ++action)
	{
	    if (m_binding_buttons_2[action]->IsActive())
		{
			isKeyBindingP2 = true;
			if (event.type == sf::Event::KeyReleased)
			{
				GetContext().player2->AssignKey(static_cast<PlayerActions>(action), event.key.code);
				m_binding_buttons_2[action]->Deactivate();
			}
			break;
		}
	}

	// If pressed button changed key bindings, update labels; otherwise consider other buttons in container
	if (isKeyBindingP1)
	{
		UpdateP1Labels();
	}
	else if (isKeyBindingP2)
	{
		UpdateP2Labels();
	}
	else
	{
		m_gui_container.HandleEvent(event);
	}

	return false;
}

void SettingsState::UpdateP1Labels()
{
	Player& player1 = *GetContext().player;

	for (std::size_t i = 0; i < m_playerActionsHalfSize; ++i)
	{
		sf::Keyboard::Key key = player1.GetAssignedKey(static_cast<PlayerActions>(i));
		m_binding_labels[i]->SetText(Utility::toString(key));
		m_binding_labels[i]->SetColour(sf::Color::Yellow);
	}
}

void SettingsState::UpdateP2Labels()
{
	Player& player2 = *GetContext().player2;

	for (std::size_t i = m_playerActionsHalfSize; i < static_cast<int>(PlayerActions::kActionCount); ++i)
	{
		sf::Keyboard::Key key = player2.GetAssignedKey(static_cast<PlayerActions>(i));
		m_binding_labels_2[i]->SetText(Utility::toString(key));
		m_binding_labels_2[i]->SetColour(sf::Color::Yellow);
	}
}


void SettingsState::AddP1ButtonLabel(PlayerActions action, float x_button_offset, float y_button_offset, float x_label_offset, const std::string& text, Context context)
{
	m_binding_buttons[static_cast<int>(action)] = std::make_shared<GUI::Button>(context);
	m_binding_buttons[static_cast<int>(action)]->setPosition(x_button_offset, y_button_offset);
	m_binding_buttons[static_cast<int>(action)]->SetText(text);
	m_binding_buttons[static_cast<int>(action)]->SetColour(sf::Color::Yellow);
	m_binding_buttons[static_cast<int>(action)]->SetToggle(true);

	m_binding_labels[static_cast<int>(action)] = std::make_shared<GUI::Label>("", *context.fonts);
	m_binding_labels[static_cast<int>(action)]->setPosition(x_label_offset, y_button_offset + 15.f);
	
	m_gui_container.Pack(m_binding_buttons[static_cast<int>(action)]);
	m_gui_container.Pack(m_binding_labels[static_cast<int>(action)]);
}

void SettingsState::AddP2ButtonLabel(PlayerActions action, float x_button_offset, float y_button_offset, float x_label_offset, const std::string& text, Context context)
{
	m_binding_buttons_2[static_cast<int>(action)] = std::make_shared<GUI::Button>(context);
	m_binding_buttons_2[static_cast<int>(action)]->setPosition(x_button_offset, y_button_offset);
	m_binding_buttons_2[static_cast<int>(action)]->SetText(text);
	m_binding_buttons_2[static_cast<int>(action)]->SetColour(sf::Color::Yellow);
	m_binding_buttons_2[static_cast<int>(action)]->SetToggle(true);

	m_binding_labels_2[static_cast<int>(action)] = std::make_shared<GUI::Label>("", *context.fonts);
	m_binding_labels_2[static_cast<int>(action)]->setPosition(x_label_offset, y_button_offset + 15.f);

	m_gui_container.Pack(m_binding_buttons_2[static_cast<int>(action)]);
	m_gui_container.Pack(m_binding_labels_2[static_cast<int>(action)]);
}