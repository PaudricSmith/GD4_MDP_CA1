#pragma once
#include "Fonts.hpp"
#include "Textures.hpp"
#include "SoundEffects.hpp"


//Forward declaration of SFML texture class
namespace sf
{
	class Texture;
	class Font;
	class SoundBuffer;
}

template<typename Resource, typename Identifier>
class ResourceHolder;

typedef ResourceHolder<sf::Texture, Textures> TextureHolder;
typedef ResourceHolder<sf::Font, Fonts> FontHolder;
typedef ResourceHolder<sf::SoundBuffer, SoundEffects> SoundBufferHolder;