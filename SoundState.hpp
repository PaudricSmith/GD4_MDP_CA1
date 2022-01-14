#pragma once

#include "State.hpp"
#include "Container.hpp"
#include "TextNode.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include "SceneNode.hpp"


class SoundState : public State
{
public:
	SoundState(StateStack& stack, Context context);

	virtual void Draw();
	virtual bool Update(sf::Time dt);
	virtual bool HandleEvent(const sf::Event& event);

private:
	sf::Sprite m_background_sprite;
	GUI::Container m_gui_container;

	int m_music_vol_value;
	int m_sfx_vol_value;

	sf::Text m_music_text;
	sf::Text m_music_vol_value_text;
	sf::Text m_sfx_text;
	sf::Text m_sfx_vol_value_text;
};