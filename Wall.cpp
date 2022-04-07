#include "Wall.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "ResourceHolder.hpp"
#include "Utility.hpp"

Wall::Wall(sf::Vector2f position, float width, float height, const TextureHolder& textures)
	: m_sprite(textures.Get(Textures::kMiddleWall))
{
	m_sprite.setPosition(position);
	m_sprite.setScale(width, height);

	Utility::CentreOrigin(m_sprite);
}

sf::FloatRect Wall::GetBoundingRect() const
{
	return GetWorldTransform().transformRect(m_sprite.getGlobalBounds());
}

void Wall::DrawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	/*sf::RectangleShape wall;
	wall.setFillColor(sf::Color::Red);
	wall.setSize(sf::Vector2f())*/

	target.draw(m_sprite, states);
}