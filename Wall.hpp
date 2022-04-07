#pragma once
#include <SFML/Graphics/Sprite.hpp>

#include "Entity.hpp"
#include "ResourceIdentifiers.hpp"

class Wall : public SceneNode
{
public:
	Wall(sf::Vector2f position, float width, float height, const TextureHolder& textures);
	virtual sf::FloatRect GetBoundingRect() const;
	virtual void DrawCurrent(sf::RenderTarget&, sf::RenderStates states) const override;

private:
	sf::Sprite m_sprite;
};

