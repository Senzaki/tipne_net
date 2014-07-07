#ifndef ANIMATOR_HPP_INCLUDED
#define ANIMATOR_HPP_INCLUDED

#include <SFML/Graphics.hpp>

class Animator
{
	public:
	Animator(sf::Sprite &sprite);

	struct Frame
	{
		float x;
		float y;
		float width;
		float height;
		float centerx;
		float centery;
		float duration;//0.f means infinity
	};

	void setFrames(const Frame *frames, unsigned int framescount);
	void update(float etime);

	private:
	void forceUpdate();
	sf::Sprite &m_sprite;
	const Frame *m_frames;
	unsigned int m_framescount;
	unsigned int m_curframe;
	float m_curtime;
};

#endif // ANIMATOR_HPP_INCLUDED
