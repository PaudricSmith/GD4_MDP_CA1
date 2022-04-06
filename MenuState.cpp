#include "MenuState.hpp"

#include <SFML/Graphics/RenderWindow.hpp>

#include "ResourceHolder.hpp"
#include "Utility.hpp"
#include "Button.hpp"


MenuState::MenuState(StateStack& stack, Context context)
: State(stack, context)
{
	sf::Texture& texture = context.textures->Get(Textures::kTitleScreen);
	m_background_sprite.setTexture(texture);

	auto play_button = std::make_shared<GUI::Button>(context);
	play_button->setPosition(400, 310);
	play_button->SetText("Local");
	play_button->SetColour(sf::Color::Yellow);
	play_button->SetCallback([this]()
	{
		RequestStackPop();
		RequestStackClear();
		RequestStackPush(StateID::kToast);
	});

	auto host_play_button = std::make_shared<GUI::Button>(context);
	host_play_button->setPosition(400, 360);
	host_play_button->SetText("Host");
	host_play_button->SetColour(sf::Color::Yellow);
	host_play_button->SetCallback([this]()
	{
		RequestStackPop();
		RequestStackClear();
		RequestStackPush(StateID::kHostGame);
	});

	auto join_play_button = std::make_shared<GUI::Button>(context);
	join_play_button->setPosition(400, 410);
	join_play_button->SetText("Join");
	join_play_button->SetColour(sf::Color::Yellow);
	join_play_button->SetCallback([this]()
	{
		RequestStackPop();
		RequestStackClear();
		RequestStackPush(StateID::kJoinGame);
	});

	auto settings_button = std::make_shared<GUI::Button>(context);
	settings_button->setPosition(400, 460);
	settings_button->SetText("Key Bindings");
	settings_button->SetColour(sf::Color::Yellow);
	settings_button->SetCallback([this]()
	{
		RequestStackPush(StateID::kSettings);
	});

	auto sound_button = std::make_shared<GUI::Button>(context);
	sound_button->setPosition(400, 510);
	sound_button->SetText("Sound");
	sound_button->SetColour(sf::Color::Yellow);
	sound_button->SetCallback([this]()
	{
		RequestStackPush(StateID::kSound);
	});

	auto exit_button = std::make_shared<GUI::Button>(context);
	exit_button->setPosition(400, 560);
	exit_button->SetColour(sf::Color::Red);
	exit_button->SetText("Exit");
	exit_button->SetCallback([this]()
	{
		RequestStackPop();
	});

	m_gui_container.Pack(play_button);
	m_gui_container.Pack(host_play_button);
	m_gui_container.Pack(join_play_button);
	m_gui_container.Pack(settings_button);
	m_gui_container.Pack(sound_button);
	m_gui_container.Pack(exit_button);


	// Play Menu Music Track
	context.musicPlayer->Play(MusicTracks::kMenuTrack);

}

void MenuState::Draw()
{
	sf::RenderWindow& window = *GetContext().window;
	window.setView(window.getDefaultView());
	window.draw(m_background_sprite);
	window.draw(m_gui_container);
}

bool MenuState::Update(sf::Time dt)
{
	return true;
}

bool MenuState::HandleEvent(const sf::Event& event)
{
	m_gui_container.HandleEvent(event);
	return false;
}