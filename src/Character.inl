template <typename... EntityArgs>
Character::Character(EntityArgs ...args):
	GameEntity(args...),
	m_state(State::Ghost),
	m_listener(nullptr)
{
	m_colobj.setRadius(DEFAULT_CHARACTER_RADIUS);
}

template <typename... EntityArgs>
Character::Character(State state, EntityArgs ...args):
	GameEntity(args...),
	m_state(state),
	m_listener(nullptr)
{
	m_colobj.setRadius(DEFAULT_CHARACTER_RADIUS);
}
