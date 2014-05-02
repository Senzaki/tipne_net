#include "DrawableCharacter.hpp"
#include "BasisChange.hpp"
#include "ResourceManager.hpp"
#include "Math.hpp"

static const unsigned int BASE_CIRCLE_VERTICES_COUNT = DEFAULT_CHARACTER_RADIUS * 32 + 1;
//TEMP
static const float TEMP_SCALE_FACTOR = 0.8f;

enum class CharacterAnimationName
{
	Normal = 0,
	Count
};

struct CharacterAnimation
{
	unsigned int fcount;
	Animator::Frame frames[16];
};

static constexpr CharacterAnimation ANIMATIONS[(int)Character::State::Count][(int)CharacterAnimationName::Count][(int)IsometricDirection::Count] =
{
	//Ghost
	{
		//Normal
		{
			//Down
			{10,
				{
					{33, 35, 82, 275, 41.f, 262.f, 0.05f},
					{33, 35, 82, 275, 41.f, 268.f, 0.05f},
					{33, 35, 82, 275, 41.f, 273.f, 0.05f},
					{33, 35, 82, 275, 41.f, 277.f, 0.05f},
					{33, 35, 82, 275, 41.f, 280.f, 0.05f},
					{33, 35, 82, 275, 41.f, 280.f, 0.05f},
					{33, 35, 82, 275, 41.f, 277.f, 0.05f},
					{33, 35, 82, 275, 41.f, 273.f, 0.05f},
					{33, 35, 82, 275, 41.f, 268.f, 0.05f},
					{33, 35, 82, 275, 41.f, 262.f, 0.05f},
				}
			},
			//DownRight
			{10,
				{
					{156, 40, 102, 278, 53.f, 255.f, 0.05f},
					{156, 40, 102, 278, 53.f, 261.f, 0.05f},
					{156, 40, 102, 278, 53.f, 266.f, 0.05f},
					{156, 40, 102, 278, 53.f, 270.f, 0.05f},
					{156, 40, 102, 278, 53.f, 273.f, 0.05f},
					{156, 40, 102, 278, 53.f, 273.f, 0.05f},
					{156, 40, 102, 278, 53.f, 270.f, 0.05f},
					{156, 40, 102, 278, 53.f, 266.f, 0.05f},
					{156, 40, 102, 278, 53.f, 261.f, 0.05f},
					{156, 40, 102, 278, 53.f, 255.f, 0.05f},
				}
			},
			//Right
			{10,
				{
					{293, 41, 69, 270, 41.f, 255.f, 0.05f},
					{293, 41, 69, 270, 41.f, 261.f, 0.05f},
					{293, 41, 69, 270, 41.f, 266.f, 0.05f},
					{293, 41, 69, 270, 41.f, 270.f, 0.05f},
					{293, 41, 69, 270, 41.f, 273.f, 0.05f},
					{293, 41, 69, 270, 41.f, 273.f, 0.05f},
					{293, 41, 69, 270, 41.f, 270.f, 0.05f},
					{293, 41, 69, 270, 41.f, 266.f, 0.05f},
					{293, 41, 69, 270, 41.f, 261.f, 0.05f},
					{293, 41, 69, 270, 41.f, 255.f, 0.05f},
				}
			},
			//UpRight
			{10,
				{
					{374, 42, 87, 270, 45.f, 250.f, 0.05f},
					{374, 42, 87, 270, 45.f, 256.f, 0.05f},
					{374, 42, 87, 270, 45.f, 261.f, 0.05f},
					{374, 42, 87, 270, 45.f, 265.f, 0.05f},
					{374, 42, 87, 270, 45.f, 268.f, 0.05f},
					{374, 42, 87, 270, 45.f, 268.f, 0.05f},
					{374, 42, 87, 270, 45.f, 265.f, 0.05f},
					{374, 42, 87, 270, 45.f, 261.f, 0.05f},
					{374, 42, 87, 270, 45.f, 256.f, 0.05f},
					{374, 42, 87, 270, 45.f, 250.f, 0.05f},
				}
			},
			//Up
			{10,
				{
					{471, 37, 82, 272, 41.f, 262.f, 0.05f},
					{471, 37, 82, 272, 41.f, 268.f, 0.05f},
					{471, 37, 82, 272, 41.f, 275.f, 0.05f},
					{471, 37, 82, 272, 41.f, 279.f, 0.05f},
					{471, 37, 82, 272, 41.f, 282.f, 0.05f},
					{471, 37, 82, 272, 41.f, 282.f, 0.05f},
					{471, 37, 82, 272, 41.f, 279.f, 0.05f},
					{471, 37, 82, 272, 41.f, 275.f, 0.05f},
					{471, 37, 82, 272, 41.f, 268.f, 0.05f},
					{471, 37, 82, 272, 41.f, 262.f, 0.05f},
				}
			},
		},
	},
};

DrawableCharacter::DrawableCharacter():
	m_animator(m_sprite),
	m_basecircle(sf::TrianglesStrip, BASE_CIRCLE_VERTICES_COUNT * 2),
	m_direction(IsometricDirection::Down),
	m_moving(false),
	m_state(Character::State::Ghost)
{
	//Setup animation
	m_sprite.setTexture(ResourceManager::getInstance().getTexture(ResourceSection::Characters, Resource::GHOST_TEX));
	m_sprite.setScale(TEMP_SCALE_FACTOR, TEMP_SCALE_FACTOR);
	resetAnimation();
	//Setup bound
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
	m_animator.update(etime);
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

void DrawableCharacter::onStateChanged(Character::State state)
{
	resetAnimation();
}

void DrawableCharacter::onPositionChanged(const sf::Vector2f &position)
{
	//Update the transform
	m_transform = sf::Transform::Identity;
	m_depth = BasisChange::gridToPixelY(position);
	m_transform.translate(BasisChange::gridToPixelX(position), m_depth);
	//Update the depth as well
	m_bounds = m_transform.transformRect(m_localbounds);
}

void DrawableCharacter::onDirectionChanged(const sf::Vector2f &direction)
{
	if(direction.x == 0.f && direction.y == 0.f)
	{
		m_moving = false;
		resetAnimation();
		return;
	}
	sf::Vector2f scale;
	//Which direction is the closest one ?
	IsometricDirection isodir;
	getDirectionInfo(direction, scale, isodir);
	if(m_direction != isodir)
	{
		m_moving = true;
		m_direction = isodir;
		m_sprite.setScale(scale * TEMP_SCALE_FACTOR);
		resetAnimation();
	}
	else if(!m_moving)
	{
		m_moving = true;
		resetAnimation();
	}
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
			positions[i] = BasisChange::gridToPixel(std::cos(angle), std::sin(angle)) * DEFAULT_CHARACTER_RADIUS;
		}
	}

	//Create the vertex array
	for(unsigned int i = 0; i < BASE_CIRCLE_VERTICES_COUNT; i++)
	{
		m_basecircle[i * 2].position = positions[i];
		m_basecircle[i * 2 + 1].position = positions[i] * 0.9f;
	}
}

void DrawableCharacter::resetAnimation()
{
	const CharacterAnimation &anim = ANIMATIONS[(int)m_state][(int)CharacterAnimationName::Normal][(int)m_direction];
	if(m_moving)
		m_animator.setFrames(anim.frames, anim.fcount);
	else
		m_animator.setFrames(anim.frames, 1);
}
