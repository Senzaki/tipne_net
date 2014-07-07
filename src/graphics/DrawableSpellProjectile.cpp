#include "DrawableSpellProjectile.hpp"
#include "Spell.hpp"
#include "ResourceManager.hpp"
#include "BasisChange.hpp"

struct SpellAnimation
{
	float height;
	unsigned int texture;
	unsigned int fcount;
	Animator::Frame frames[16];
};

static constexpr SpellAnimation ANIMATIONS[(unsigned int)Spell::Appearance::Count] =
{
	//Basic
	{
		120.f,
		Resource::BASIC_SPELL,
		4,
		{
			{0, 0, 64, 64, 32.f, 32.f, 0.1f},
			{64, 0, 64, 64, 32.f, 32.f, 0.1f},
			{128, 0, 64, 64, 32.f, 32.f, 0.1f},
			{192, 0, 64, 64, 32.f, 32.f, 0.1f},
		}
	}
};

DrawableSpellProjectile::DrawableSpellProjectile(SpellProjectile *projectile):
	m_animator(m_sprite)
{
	projectile->setStateListener(this);
	initAppearance(projectile->getAppearance());
}

DrawableSpellProjectile::~DrawableSpellProjectile()
{

}

void DrawableSpellProjectile::update(float etime)
{
	m_animator.update(etime);
}

void DrawableSpellProjectile::draw(sf::RenderWindow &window)
{
	window.draw(m_sprite);
}

float DrawableSpellProjectile::getDepth() const
{
	return m_sprite.getPosition().y + m_height;
}

bool DrawableSpellProjectile::isContainedIn(const sf::FloatRect &rect) const
{
	return rect.intersects(m_sprite.getGlobalBounds());
}

void DrawableSpellProjectile::onPositionChanged(const sf::Vector2f &position)
{
	m_sprite.setPosition(BasisChange::gridToPixelX(position), BasisChange::gridToPixelY(position) - m_height);
}

void DrawableSpellProjectile::initAppearance(sf::Uint16 appearance)
{
	const SpellAnimation &animation = ANIMATIONS[appearance];
	m_sprite.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Game, animation.texture));
	m_animator.setFrames(animation.frames, animation.fcount);
	m_height = animation.height;
}
