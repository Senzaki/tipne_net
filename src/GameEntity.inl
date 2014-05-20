class Character;

sf::Uint16 GameEntity::getId() const
{
	return m_id;
}

void GameEntity::setOwner(sf::Uint8 owner)
{
	m_owner = owner;
}

sf::Uint8 GameEntity::getOwner() const
{
	return m_owner;
}

void GameEntity::setFullySimulated(bool fullysimulated)
{
	m_fullysimulated = fullysimulated;
}

bool GameEntity::isFullySimulated() const
{
	return m_fullysimulated;
}

void GameEntity::setInterpolationTime(float interpolationtime)
{
	m_colobj.setInterpolationTime(interpolationtime);
}

float GameEntity::getInterpolationTime() const
{
	return m_colobj.getInterpolationTime();
}

void GameEntity::setSimulator(GameSimulator *simulator)
{
	m_simulator = simulator;
}

void GameEntity::setCollisionManager(CollisionManager *colmgr)
{
	//Remove the former collision manager (if any)
	if(m_colobj.getCollisionManager())
		m_colobj.getCollisionManager()->detach(&m_colobj);
	//Attach the object to the new one
	if(colmgr)
		colmgr->attach(&m_colobj);
}

void GameEntity::setPosition(float x, float y)
{
	m_colobj.setPosition(x, y);
}

void GameEntity::forcePosition(float x, float y)
{
	m_colobj.forcePosition(x, y);
}

sf::Vector2f GameEntity::getPosition() const
{
	return m_colobj.getPosition();
}
