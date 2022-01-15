#include "SoundState.hpp"

#include <iostream>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "ResourceHolder.hpp"
#include "DataTables.hpp"
#include "Utility.hpp"
#include "Button.hpp"

#include "MenuState.hpp"


SoundState::SoundState(StateStack& stack, Context context)
	: State(stack, context)
	, m_music_vol_value(100)
	, m_sfx_vol_value(100)
	, m_music_text("", context.fonts->Get(Fonts::Main), 24)
	, m_sfx_text("", context.fonts->Get(Fonts::Main), 24)
	, m_music_vol_value_text("", context.fonts->Get(Fonts::Main), 20)
	, m_sfx_vol_value_text("", context.fonts->Get(Fonts::Main), 20)
	, m_music_player(*context.musicPlayer)
	, m_sfx_player(*context.sounds)
{
	
	// SET BACKGROUND SPRITE
	sf::Texture& texture = context.textures->Get(Textures::kTitleScreen);
	m_background_sprite.setTexture(texture);

	// SET TEXT
	m_music_text.setString("Music Volume");
	m_music_text.setFillColor(sf::Color::Blue);
	m_music_text.setPosition(420, 250);

	m_sfx_text.setString("SFX Volume");
	m_sfx_text.setFillColor(sf::Color::Blue);
	m_sfx_text.setPosition(430, 400);

	m_music_vol_value_text.setString(std::to_string(m_music_player.GetVolume()));
	m_music_vol_value_text.setFillColor(sf::Color::White);
	m_music_vol_value_text.setPosition(485, 315);

	m_sfx_vol_value_text.setString(std::to_string(m_sfx_player.GetVolume()));
	m_sfx_vol_value_text.setFillColor(sf::Color::White);
	m_sfx_vol_value_text.setPosition(485, 465);


	// MUSIC BUTTONS
	auto music_button_increase = std::make_shared<GUI::Button>(context);
	music_button_increase->setPosition(550, 300);
	music_button_increase->SetText("+");
	music_button_increase->SetCallback([this]()
		{
			m_music_vol_value = m_music_player.GetVolume();
			m_music_vol_value_text.setString(std::to_string(m_music_vol_value));

			if (m_music_vol_value < 100)
			{
				m_music_vol_value += 10;

				if (m_music_vol_value > 100)
				{
					m_music_vol_value = 100;
				}

				m_music_vol_value_text.setString(std::to_string(m_music_vol_value));

				// Set Music volume
				m_music_player.SetVolume(m_music_vol_value);
			}
		});

	auto music_button_decrease = std::make_shared<GUI::Button>(context);
	music_button_decrease->setPosition(250, 300);
	music_button_decrease->SetText("-");
	music_button_decrease->SetCallback([this]()
		{
			m_music_vol_value = m_music_player.GetVolume();
			m_music_vol_value_text.setString(std::to_string(m_music_vol_value));

			if (m_music_vol_value > 0)
			{
				m_music_vol_value -= 10;

				if (m_music_vol_value < 0)
				{
					m_music_vol_value = 0;
				}

				m_music_vol_value_text.setString(std::to_string(m_music_vol_value));

				// Set Music volume
				m_music_player.SetVolume(m_music_vol_value);
			}
		});

	// SFX BUTTONS
	auto sfx_button_increase = std::make_shared<GUI::Button>(context);
	sfx_button_increase->setPosition(550, 450);
	sfx_button_increase->SetText("+");
	sfx_button_increase->SetCallback([this]()
		{
			m_sfx_vol_value = m_sfx_player.GetVolume();
			m_sfx_vol_value_text.setString(std::to_string(m_sfx_vol_value));

			if (m_sfx_vol_value < 100)
			{
				m_sfx_vol_value += 10;

				if (m_sfx_vol_value > 100)
				{
					m_sfx_vol_value = 100;
				}

				m_sfx_vol_value_text.setString(std::to_string(m_sfx_vol_value));

				// Set SFX volume
				m_sfx_player.SetVolume(m_sfx_vol_value);
			}
		});

	auto sfx_button_decrease = std::make_shared<GUI::Button>(context);
	sfx_button_decrease->setPosition(250, 450);
	sfx_button_decrease->SetText("-");
	sfx_button_decrease->SetCallback([this]()
		{
			m_sfx_vol_value = m_sfx_player.GetVolume();
			m_sfx_vol_value_text.setString(std::to_string(m_sfx_vol_value));

			if (m_sfx_vol_value > 0)
			{
				m_sfx_vol_value -= 10;

				if (m_sfx_vol_value < 0)
				{
					m_sfx_vol_value = 0;
				}

				m_sfx_vol_value_text.setString(std::to_string(m_sfx_vol_value));

				// Set SFX volume
				m_sfx_player.SetVolume(m_sfx_vol_value);
			}
		});

	// BACK BUTTON
	auto back_button = std::make_shared<GUI::Button>(context);
	back_button->setPosition(400.f, 620.f);
	back_button->SetText("Back");
	back_button->SetCallback(std::bind(&SoundState::RequestStackPop, this));


	// PACK BUTTONS TO GUI
	m_gui_container.Pack(music_button_decrease);
	m_gui_container.Pack(sfx_button_decrease);
	m_gui_container.Pack(music_button_increase);
	m_gui_container.Pack(sfx_button_increase);
	m_gui_container.Pack(back_button);

}

void SoundState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());
	window.draw(m_background_sprite);
	window.draw(m_music_text);
	window.draw(m_music_vol_value_text);
	window.draw(m_sfx_text);
	window.draw(m_sfx_vol_value_text);
	window.draw(m_gui_container);
}

bool SoundState::Update(sf::Time dt)
{
	return true;
}

bool SoundState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}