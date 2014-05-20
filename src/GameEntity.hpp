#ifndef GAMEENTITY_HPP_INCLUDED
#define GAMEENTITY_HPP_INCLUDED

#include "Player.hpp"
#include "CollisionManager.hpp"

class GameSimulator;

constexpr const sf::Uint16 NO_ENTITY_ID = 0;

class GameEntity
{
	public:
	GameEntity(sf::Uint16 id, bool fullysimulated = true, float interpolationtime = 0.f, sf::Uint8 owner = NEUTRAL_PLAYER, sf::Vector2f position = sf::Vector2f());
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
	inline void setSimulator(GameSimulator *simulator);

	inline void setCollisionManager(CollisionManager *colmgr);
	inline void setPosition(float x, float y);//Uses interpolation
	inline void forcePosition(float x, float y);//Does not use interpolation
	inline sf::Vector2f getPosition() const;

	protected:
	CollisionObject m_colobj;
	bool m_fullysimulated;
	GameSimulator *m_simulator;

	private:
	sf::Uint16 m_id;
	sf::Uint8 m_owner;
};

#include "GameEntity.inl"

#endif // GAMEENTITY_HPP_INCLUDED
