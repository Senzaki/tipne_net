#include "Animator.hpp"

Animator::Animator(sf::Sprite &sprite):
	m_sprite(sprite),
	m_frames(nullptr)
{

}

void Animator::setFrames(const Frame *frames, unsigned int framescount)
{
	m_framescount = framescount;
	m_curtime = 0.f;
	m_curframe = 0;
	//Is there at least one frame ?
	if(framescount == 0)
		m_frames = nullptr;
	else
	{
		m_frames = frames;
		//Init the first frame
		if(frames)
			forceUpdate();
	}
}

void Animator::update(float etime)
{
	if(!m_frames)
		return;
	if(m_frames[m_curframe].duration == 0.f)
		return;
	//Find the new frame
	m_curtime += etime;
	bool changed = false;
	while(m_curtime >= m_frames[m_curframe].duration)
	{
		m_curtime -= m_frames[m_curframe].duration;
		changed = true;
		m_curframe = (m_curframe + 1) % m_framescount;
	}
	//If the new frame has changed, show the new frame
	if(changed)
		forceUpdate();
}

void Animator::forceUpdate()
{
	//Apply the current frame
	const Frame &frame = m_frames[m_curframe];
	m_sprite.setTextureRect(sf::IntRect(frame.x, frame.y, frame.width, frame.height));
	m_sprite.setOrigin(frame.centerx, frame.centery);
}
