#ifndef SPELL_HPP_INCLUDED
#define SPELL_HPP_INCLUDED

#include "Character.hpp"
#include "IDCreator.hpp"

struct Spell
{
	enum class Type
	{
		None = 0,
		LineSpell
	};
	enum class Appearance : sf::Uint16
	{
		Basic = 0,
		Count
	};

	Character::State state;
	sf::Uint8 id;

	union
	{
		struct
		{
			float x;
			float y;
		} targetpoint;
	};

	Spell::Type getAssociatedType() const;

	bool isSameSpell(const Spell &other) const;
	bool castSpell(RoundState &round, IDCreator<sf::Uint16> &idcreator, Character *character) const;
};

#endif // SPELL_HPP_INCLUDED
