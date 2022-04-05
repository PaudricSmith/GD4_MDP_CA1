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

	for (std::size_t x = 0; x < 2; ++x)
	{
		// Build key binding buttons and labels
		AddButtonLabel(static_cast<int>(PlayerActions::kMoveForwards), x, 0, "Move Forwards", context);
		AddButtonLabel(static_cast<int>(PlayerActions::kMoveBackwards), x, 1, "Move Backwards", context);
		AddButtonLabel(static_cast<int>(PlayerActions::kRotateLeft), x, 2, "Rotate Left", context);
		AddButtonLabel(static_cast<int>(PlayerActions::kRotateRight), x, 3, "Rotate Right", context);
		AddButtonLabel(static_cast<int>(PlayerActions::kRotateCannonLeft), x, 4, "Rotate Gun Left", context);
		AddButtonLabel(static_cast<int>(PlayerActions::kRotateCannonRight), x, 5, "Rotate Gun Right", context);
		AddButtonLabel(static_cast<int>(PlayerActions::kFire), x, 6, "Fire", context);
		AddButtonLabel(static_cast<int>(PlayerActions::kGuidedMissile), x, 7, "Missile", context);
	}
	

	//// Build key binding buttons and labels Player 2
	//AddP2ButtonLabel(PlayerActions::kMoveForwards2, 600.0f, 160.f, 530.0f, "Move Forwards", context);
	//AddP2ButtonLabel(PlayerActions::kMoveBackwards2, 600.0f, 210.f, 530.0f, "Move Backwards", context);
	//AddP2ButtonLabel(PlayerActions::kRotateLeft2, 600.0f, 260.f, 530.0f, "Rotate Left", context);
	//AddP2ButtonLabel(PlayerActions::kRotateRight2, 600.0f, 310.f, 530.0f, "Rotate Right", context);
	//AddP2ButtonLabel(PlayerActions::kRotateCannonLeft2, 600.0f, 360.f, 530.0f, "Rotate Gun Left", context);
	//AddP2ButtonLabel(PlayerActions::kRotateCannonRight2, 600.0f, 410.f, 530.0f, "Rotate Gun Right", context);
	//AddP2ButtonLabel(PlayerActions::kFire2, 600.0f, 460.f, 530.0f, "Fire", context);
	//AddP2ButtonLabel(PlayerActions::kGuidedMissile2, 600.0f, 510.f, 530.0f, "Missile", context);

	//UpdateP1Labels();
	//UpdateP2Labels();

	UpdateLabels();

	auto back_button = std::make_shared<GUI::Button>(context);
	back_button->setPosition(420.f, 600.f);
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
	bool is_key_binding = false;

	// Iterate through all key binding buttons to see if they are being pressed, waiting for the user to enter a key
	
	for (std::size_t i = 0; i < 2 * (static_cast<int>(PlayerActions::kActionCount)); ++i)
	{
		if (m_binding_buttons[i]->IsActive())
		{
			is_key_binding = true;
			if (event.type == sf::Event::KeyReleased)
			{
				// Player 1
				if (i < static_cast<int>(PlayerActions::kActionCount))
					GetContext().keys1->AssignKey(static_cast<PlayerActions>(i), event.key.code);

				// Player 2
				else
					GetContext().keys2->AssignKey(static_cast<PlayerActions>(i - static_cast<int>(PlayerActions::kActionCount)), event.key.code);

				m_binding_buttons[i]->Deactivate();
			}
			break;
		}
	}

	// If pressed button changed key bindings, update labels; otherwise consider other buttons in container
	if (is_key_binding)
		UpdateLabels();
	else
		m_gui_container.HandleEvent(event);

	return false;

}

void SettingsState::UpdateLabels()
{
	for (std::size_t i = 0; i < static_cast<int>(PlayerActions::kActionCount); ++i)
	{
		auto action = static_cast<PlayerActions>(i);

		// Get keys of both players
		sf::Keyboard::Key key1 = GetContext().keys1->GetAssignedKey(action);
		sf::Keyboard::Key key2 = GetContext().keys2->GetAssignedKey(action);

		// Assign both key strings to labels
		m_binding_labels[i]->SetText(Utility::toString(key1));
		m_binding_labels[i + static_cast<int>(PlayerActions::kActionCount)]->SetText(Utility::toString(key2));
	}
}

void SettingsState::AddButtonLabel(std::size_t index, std::size_t x, std::size_t y, const std::string& text, Context context)
{
	// For x==0, start at index 0, otherwise start at half of array
	index += static_cast<int>(PlayerActions::kActionCount) * x;

	m_binding_buttons[index] = std::make_shared<GUI::Button>(context);
	m_binding_buttons[index]->setPosition(380.f * x + 210.f, 50.f * y + 160.f);
	m_binding_buttons[index]->SetText(text);
	m_binding_buttons[index]->SetToggle(true);

	m_binding_labels[index] = std::make_shared<GUI::Label>("", *context.fonts);
	m_binding_labels[index]->setPosition(110.f * x + 420.f, 50.f * y + 175.f);

	m_gui_container.Pack(m_binding_buttons[index]);
	m_gui_container.Pack(m_binding_labels[index]);
}