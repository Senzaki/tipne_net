#ifndef GAMEENTITY_HPP_INCLUDED
#define GAMEENTITY_HPP_INCLUDED

#include "Player.hpp"
#include "CollisionManager.hpp"

class RoundState;

constexpr const sf::Uint16 NO_ENTITY_ID = 0;

class GameEntity
{
	public:
	GameEntity(RoundState &round, sf::Uint16 id);
	virtual ~GameEntity();

	GameEntity(const GameEntity &) = delete;
	GameEntity &operator=(const GameEntity &) = delete;

	virtual void update(float etime) = 0;

	inline sf::Uint16 getId() const;

	inline void setOwner(sf::Uint8 owner);
	inline sf::Uint8 getOwner() const;

	//Should it be fully simulated, or should we wait for a "server" (or any full-auth system) to tell it what to do ? (note : it is possible to combine both -> extrapolation)
	inline void setFullySimulated(bool fullysimulated);
	inline bool isFullySimulated() const;
	inline void setInterpolationTime(float interpolationtime);
	inline float getInterpolationTime() const;

	inline void setCollisionManager(CollisionManager *colmgr);
	inline void setPosition(float x, float y);//Uses interpolation
	inline void setPosition(const sf::Vector2f &pos);//Uses interpolation
	inline void forcePosition(float x, float y);//Does not use interpolation
	inline void forcePosition(const sf::Vector2f &pos);//Does not use interpolation
	inline sf::Vector2f getPosition() const;
	virtual void onCollision(CollisionObject *other) { }

	inline unsigned int getLastSnapshotId() const;
	inline void setLastSnapshotId(unsigned int id);

	protected:
	CollisionObject m_colobj;
	bool m_fullysimulated;
	RoundState &m_round;

	private:
	sf::Uint16 m_id;
	sf::Uint8 m_owner;
	unsigned int m_lastsnapshot;
};

#include "GameEntity.inl"

#endif // GAMEENTITY_HPP_INCLUDED
