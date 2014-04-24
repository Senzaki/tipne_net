#include "DrawableCharacter.hpp"
#include "BasisChange.hpp"
#include "ResourceManager.hpp"
#include "Math.hpp"

static const unsigned int BASE_CIRCLE_VERTICES_COUNT = 24;

DrawableCharacter::DrawableCharacter():
	m_basecircle(sf::TrianglesStrip, BASE_CIRCLE_VERTICES_COUNT * 2)
{
	//TEMP
	m_sprite.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Characters, Resource::GHOST_TEX));
	m_sprite.setTextureRect(sf::IntRect(33, 35, 82, 275));
	m_sprite.setOrigin(41.f, 262.f);
	m_localbounds = m_sprite.getGlobalBounds();
	m_localbounds.height += 40.f;

	initializeBaseCircle();
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
	window.draw(m_basecircle, m_transform);
	window.draw(m_sprite, m_transform);
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
	return m_bounds.intersects(rect);
}

void DrawableCharacter::onPositionChanged(sf::Vector2f position)
{
	//Update the transform
	m_transform = sf::Transform::Identity;
	m_transform.translate(BasisChange::gridToPixel(position));
	//Update the depth as well
	m_bounds = m_transform.transformRect(m_localbounds);
	m_depth = m_bounds.top + m_bounds.width;
}

void DrawableCharacter::onStatusChanged(Character::State state)
{

}

void DrawableCharacter::initializeBaseCircle()
{
	static bool initialized = false;
	static sf::Vector2f positions[BASE_CIRCLE_VERTICES_COUNT];

	//Positions should be computed only once (sin/cos is expensive)
	if(!initialized)
	{
		float angle;
		const float interval = 2.f * PI / static_cast<float>(BASE_CIRCLE_VERTICES_COUNT - 1);
		for(unsigned int i = 0; i < BASE_CIRCLE_VERTICES_COUNT; i++)
		{
			angle = static_cast<float>(i) * interval;
			positions[i] = BasisChange::gridToPixel(std::cos(angle), std::sin(angle)) / 2.f;
		}
	}

	//Create the vertex array
	for(unsigned int i = 0; i < BASE_CIRCLE_VERTICES_COUNT; i++)
	{
		m_basecircle[i * 2].position = positions[i];
		m_basecircle[i * 2 + 1].position = positions[i] * 0.9f;
	}
}
