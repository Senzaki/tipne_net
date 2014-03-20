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

void DrawableCharacter::onPositionChanged(sf::Vector2f position)
{
	m_sprite.setPosition(BasisChange::gridToPixel(position));
}

void DrawableCharacter::onStatusChanged(Character::State state)
{

}
