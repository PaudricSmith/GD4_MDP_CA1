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
	
	sf::Sprite m_background_sprite;

	sf::Text m_ready_text;
	sf::Text m_steady_text;
	sf::Text m_go_text;
	sf::Time m_elapsed_time;

	bool m_draw_text_1;
	bool m_draw_text_2;
	bool m_draw_text_3;
};