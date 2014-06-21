#include "GameScreen.hpp"
#include "Application.hpp"
#include "KeyMap.hpp"
#include "BasisChange.hpp"
#include "DrawableCharacter.hpp"
#include "DrawableSpellProjectile.hpp"
#include "make_unique.hpp"

GameScreen::GameScreen(float vratio, float xyratio, GameSimulator *simulator):
	m_camera(sf::FloatRect(-100.f, -DEFAULT_SCREEN_HEIGHT / 2, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_seen(sf::FloatRect(-100.f, -DEFAULT_SCREEN_HEIGHT / 2, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_vratio(vratio),
	m_xyratio(xyratio),
	m_otherdirpressed(false, false),
	m_curaction(Normal)
{
	setSimulator(simulator);
}

GameScreen::~GameScreen()
{

}

void GameScreen::setSimulator(GameSimulator *simulator)
{
	m_simulator = simulator;
	if(simulator)
		simulator->setStateListener(this);
}

bool GameScreen::update(float etime)
{
	bool toreturn = true;
	//Update the simulator (and return false if the simulation is over)
	if(m_simulator)
		toreturn = m_simulator->update(etime);
	//Update the camera
	const Character *player = static_cast<const GameSimulator *>(m_simulator)->getOwnCharacter();
	if(player)
	{
		sf::Vector2f camcenter = BasisChange::gridToPixel(player->getPosition());
		m_camera.setCenter(camcenter);
        m_seen.left = camcenter.x - m_seen.width / 2.f;
        m_seen.top = camcenter.y - m_seen.height / 2.f;
		m_map.update(etime, true, player->getPosition());
	}
	else
		m_map.update(etime);
	//Update the graphical entities
	for(std::pair<const sf::Uint16, std::unique_ptr<DrawableEntity>> &entity : m_entities)
		entity.second->update(etime);
	return toreturn;
}

void GameScreen::draw(sf::RenderWindow &window)
{
	//Save the old view and use the game camera
	sf::View oldview = window.getView();
	window.setView(m_camera);
	//Put the graphical entities that need to be drawn in a list
	std::list<DrawableEntity *> todraw;
	//Draw the map & add map entities to thez list
	m_map.draw(window, m_seen, todraw);
	//Add the entities to the list
	for(sf::Uint16 id : m_visibleentities)
	{
		try
		{
			DrawableEntity *entity = m_entities.at(id).get();
			if(entity->isContainedIn(m_seen))
				todraw.emplace_back(entity);
		}
		catch(const std::out_of_range &)
		{

		}
	}
	//Sort the entities by depth
	todraw.sort(DrawableEntity::isDepthLower);
	//Draw all the entities
	for(DrawableEntity *entity : todraw)
		entity->draw(window);
	//Go back to the old view
	window.setView(oldview);
}

void GameScreen::onKeyPressed(const sf::Event::KeyEvent &evt)
{
	//Interpret key
	KeyAction action = KeyMap::getInstance().getActionForKey(evt);
	switch(action)
	{
		case KeyAction::None:
		case KeyAction::Count:
			return;

		case KeyAction::Left:
			//If the current direction is right, remember that this key was pressed
			if(m_direction.x == GRID_WIDTH)
				m_otherdirpressed.x = true;
			//Set the direction to left
			m_direction.x = -GRID_WIDTH;
			updateDirection();
			break;

		case KeyAction::Right:
			//See case KeyAction::Left
			if(m_direction.x == -GRID_WIDTH)
				m_otherdirpressed.x = true;
			m_direction.x = GRID_WIDTH;
			updateDirection();
			break;

		case KeyAction::Up:
			//See case KeyAction::Left
			if(m_direction.y == GRID_HEIGHT)
				m_otherdirpressed.y = true;
			m_direction.y = -GRID_HEIGHT;
			updateDirection();
			break;

		case KeyAction::Down:
			//See case KeyAction::Left
			if(m_direction.y == -GRID_HEIGHT)
				m_otherdirpressed.y = true;
			m_direction.y = GRID_HEIGHT;
			updateDirection();
			break;

		case KeyAction::Spell1:
			startCastingSpell(0);
			break;
	}
}

void GameScreen::onKeyReleased(const sf::Event::KeyEvent &evt)
{
	//Interpret key
	KeyAction action = KeyMap::getInstance().getActionForKey(evt);
	switch(action)
	{
		case KeyAction::None:
		case KeyAction::Count:
			return;

		case KeyAction::Left:
			//If moving to the left
			if(m_direction.x == -GRID_WIDTH)
			{
				//If the right was pressed, go back to right
				if(m_otherdirpressed.x)
					m_direction.x = GRID_WIDTH;
				else
					m_direction.x = 0.f;
				updateDirection();
			}
			//No matter what, one of the two keys has been released, so the other direction is not pressed !
			m_otherdirpressed.x = false;
			updateDirection();
			break;

		case KeyAction::Right:
			//See case KeyAction::Left
			if(m_direction.x == GRID_WIDTH)
			{
				if(m_otherdirpressed.x)
					m_direction.x = -GRID_WIDTH;
				else
					m_direction.x = 0.f;
			}
			m_otherdirpressed.x = false;
			updateDirection();
			break;

		case KeyAction::Up:
			//See case KeyAction::Left
			if(m_direction.y == -GRID_HEIGHT)
			{
				if(m_otherdirpressed.y)
					m_direction.y = GRID_HEIGHT;
				else
					m_direction.y = 0.f;
			}
			m_otherdirpressed.y = false;
			updateDirection();
			break;

		case KeyAction::Down:
			//See case KeyAction::Left
			if(m_direction.y == GRID_HEIGHT)
			{
				if(m_otherdirpressed.y)
					m_direction.y = -GRID_HEIGHT;
				else
					m_direction.y = 0.f;
			}
			m_otherdirpressed.y = false;
			updateDirection();
			break;

		case KeyAction::Spell1:
			break;
	}
}

void GameScreen::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	const sf::Vector2f worldpixelpos(m_seen.left + evt.x / m_vratio, m_seen.top + evt.y / m_vratio);
	switch(m_curaction)
	{
		case Normal:
			return;

		case AcquiringTargetPoint:
			if(evt.button == sf::Mouse::Left)
			{
				//Compute targeted point
				m_curspell.targetpoint.x = BasisChange::pixelToGridX(worldpixelpos);
				m_curspell.targetpoint.y = BasisChange::pixelToGridY(worldpixelpos);
				if(m_simulator)
					m_simulator->selfCastSpell(m_curspell);
				//Spell was casted, back to normal interface
				m_curaction = Normal;
			}
			else if(evt.button == sf::Mouse::Right)//Spell casting cancelled
				m_curaction = Normal;
			break;
	}
}

void GameScreen::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{

}

void GameScreen::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{

}

void GameScreen::onNewPlayer(Player &player)
{

}

void GameScreen::onPlayerLeft(Player &player, sf::Uint8 reason)
{

}

void GameScreen::onNewEntity(GameEntity *entity)
{
	//Add a drawable entity depending on the type of entity
	if(Character *character = dynamic_cast<Character *>(entity))
		m_entities.emplace(character->getId(), make_unique<DrawableCharacter>(character));
	else if(SpellProjectile *spell = dynamic_cast<SpellProjectile *>(entity))
		m_entities.emplace(spell->getId(), make_unique<DrawableSpellProjectile>(spell));
}

void GameScreen::onEntityRemoved(GameEntity *entity)
{
	//Remove the associated drawable entity
	auto it = m_entities.find(entity->getId());
	if(it != m_entities.end())
		m_entities.erase(it);
}

void GameScreen::onMapLoaded(const Map &map)
{
	m_map.setMap(map);
}

void GameScreen::onVisibleEntitiesChanged(std::list<sf::Uint16> &&entities)
{
	m_visibleentities = std::move(entities);
}

void GameScreen::updateDirection()
{
	if(m_simulator)
		m_simulator->selfSetDirection(BasisChange::pixelToGrid(m_direction));
}

void GameScreen::startCastingSpell(sf::Uint8 id)
{
	if(m_simulator)
	{
		if(const Character *character = static_cast<const GameSimulator *>(m_simulator)->getOwnCharacter())
		{
			Spell spell;
			spell.state = character->getState();
			spell.id = id;
			//If we are currently getting info for the same spell, don't restart
			if(m_curaction != Normal && spell.isSameSpell(m_curspell))
				return;
			Spell::Type type = spell.getAssociatedType();
			//Don't cancel the current spell if a wrong key is pressed
			if(type == Spell::Type::None)
				return;
			m_curspell = spell;
			switch(type)
			{
				case Spell::Type::None:
					return;//Impossible

				case Spell::Type::LineSpell:
					m_curaction = AcquiringTargetPoint;
					break;
			}
		}
	}
}
