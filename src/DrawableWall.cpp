#include "DrawableWall.hpp"
#include "BasisChange.hpp"
#include "DrawableTilesGroup.hpp"
#include "ResourceManager.hpp"
#include "Animator.hpp"
#include <iostream>

static const unsigned int WALL_INFO_SIZE = 0x1;

static constexpr GraphTileInfo WALL_INFO[WALL_INFO_SIZE] =
{
	//FIRST_WALL_APPEARANCE + ...
	//1
	{Resource::BASE_WALLS_TEX, 1, {
		{0.f, 0.f, 120.f, 191.f, 0.f, 161.f}
	}},
};

DrawableWall::DrawableWall(const sf::Vector2u &position, sf::Uint16 appearance)
{
	appearance -= FIRST_WALL_APPEARANCE;
	//Valid appearance ?
	if(appearance >= WALL_INFO_SIZE)
		std::cerr << "Error while loading tiles : invalid wall appearance : " << appearance << '.' << std::endl;
	else
	{
		//Set texture
		m_sprite.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Map, WALL_INFO[appearance].texture));
		//Set the right part of the texture + the origin
		Animator animator(m_sprite);
		animator.setFrames(WALL_INFO[appearance].frames, WALL_INFO[appearance].framescount);
	}
	//Set position & depth
	m_sprite.setPosition(BasisChange::gridToPixel(position));
	m_depth = m_sprite.getPosition().y;
}

DrawableWall::~DrawableWall()
{

}

float DrawableWall::getDepth() const
{
	return m_depth;
}

bool DrawableWall::isContainedIn(const sf::FloatRect &rect) const
{
	return m_sprite.getGlobalBounds().intersects(rect);
}

void DrawableWall::draw(sf::RenderWindow &window)
{
	window.draw(m_sprite);
}
