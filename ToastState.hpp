#pragma once

#include "State.hpp"

#include "World.hpp"

#include <SFML/Graphics/Text.hpp>


class ToastState : public State
{
public:
	ToastState(StateStack& stack, Context context);

	virtual void Draw();
	virtual bool Update(sf::Time dt);
	virtual bool HandleEvent(const sf::Event& event);

private:
	World m_world;

	SoundPlayer& m_sfx_player;
	
	sf::Sprite m_background_sprite;

	sf::Text m_ready_text;
	sf::Text m_steady_text;
	sf::Text m_go_text;
	sf::Time m_elapsed_time;

	bool m_beep_1_played;
	bool m_beep_2_played;
};