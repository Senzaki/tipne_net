#include "Spell.hpp"
#include "GameSimulator.hpp"
#include "IDCreator.hpp"
#include "LineDamageSpell.hpp"

static const sf::Uint8 SPELLS_COUNT_PER_STATE = 5;

static Spell::Type SPELLS_TYPE[(unsigned int)Character::State::Count][SPELLS_COUNT_PER_STATE] =
{
	//Character::Ghost
	{
		Spell::Type::LineSpell
	}
};

Spell::Type Spell::getAssociatedType() const
{
	if(id >= SPELLS_COUNT_PER_STATE)
		return Spell::Type::None;
	return SPELLS_TYPE[static_cast<unsigned int>(state)][id];
}

bool Spell::isSameSpell(const Spell &other) const
{
	return id == other.id && state == other.state;
}

bool Spell::castSpell(RoundState &round, IDCreator<sf::Uint16> &idcreator, Character *character) const
{
	if(state != character->getState() || getAssociatedType() == Spell::Type::None)
	{
#ifndef NEDBUG
		std::cerr << "[DEBUG]castSpell() function called with invalid spell." << std::endl;
#endif
		return false;
	}
	switch(state)
	{
		case Character::State::Ghost:
			{
				//Create the projectile
				LineDamageSpell *projectile = round.addEntity<LineDamageSpell>(round, idcreator.getNewID(), (sf::Uint16)Spell::Appearance::Basic, character, 0.2f, 1);
				projectile->setOwner(character->getOwner());
				sf::Vector2f startpos = character->getPosition();
				projectile->setPosition(startpos);
				//Compute the speed vector (direction then length)
				sf::Vector2f speed(targetpoint.x - startpos.x, targetpoint.y - startpos.y);
				speed = speed / std::sqrt(speed.x * speed.x + speed.y * speed.y) * 5.f;
				projectile->setSpeed(speed);
			}
			break;

		case Character::State::Count:
			return false;
	}
	return true;
}
