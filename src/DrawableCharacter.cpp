#include "DrawableCharacter.hpp"
#include "BasisChange.hpp"
#include "ResourceManager.hpp"

DrawableCharacter::DrawableCharacter()
{
	//TEMP
	m_sprite.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Characters, Resource::GHOST_TEX));
	m_sprite.setTextureRect(sf::IntRect(33, 35, 82, 275));
	m_sprite.setOrigin(41.f, 275.f);
}

DrawableCharacter::DrawableCharacter(Character &character):
	DrawableCharacter()
{
	character.setCharacterStateListener(this);
}

DrawableCharacter::~DrawableCharacter()
{

}

void DrawableCharacter::update(float etime)
{

}

void DrawableCharacter::draw(sf::RenderWindow &window)
{
	window.draw(m_sprite);
}

sf::Vector2f DrawableCharacter::getPosition() const
{
	return m_sprite.getPosition();
}

float DrawableCharacter::getDepth() const
{
	return m_depth;
}

bool DrawableCharacter::isContainedIn(const sf::FloatRect &rect) const
{
	return m_sprite.getGlobalBounds().intersects(rect);
}

void DrawableCharacter::onPositionChanged(sf::Vector2f position)
{
	m_sprite.setPosition(BasisChange::gridToPixel(position));
	//Update the depth as well
	sf::FloatRect bounds = m_sprite.getGlobalBounds();
	m_depth = bounds.top + bounds.width;
}

void DrawableCharacter::onStatusChanged(Character::State state)
{

}
