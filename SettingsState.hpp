#pragma once
#include "State.hpp"

#include "State.hpp"
#include "Player.hpp"
#include "Container.hpp"
#include "Button.hpp"
#include "Label.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <array>


class SettingsState : public State
{
public:
	SettingsState(StateStack& stack, Context context);

	virtual void Draw();
	virtual bool Update(sf::Time dt);
	virtual bool HandleEvent(const sf::Event& event);


private:
	void UpdateP1Labels();
	void UpdateP2Labels();
	void AddP1ButtonLabel(PlayerActions action, float x_offset, float y_offset, float x_label_offset, const std::string& text, Context context);
	void AddP2ButtonLabel(PlayerActions action, float x_offset, float y_offset, float x_label_offset, const std::string& text, Context context);


private:
	int m_playerActionsHalfSize;

	sf::Sprite m_background_sprite;
	GUI::Container m_gui_container;
	std::array<GUI::Button::Ptr, static_cast<int>(PlayerActions::kActionCount)> m_binding_buttons;
	std::array<GUI::Button::Ptr, static_cast<int>(PlayerActions::kActionCount)> m_binding_buttons_2;
	std::array<GUI::Label::Ptr, static_cast<int>(PlayerActions::kActionCount)> m_binding_labels;
	std::array<GUI::Label::Ptr, static_cast<int>(PlayerActions::kActionCount)> m_binding_labels_2;
};