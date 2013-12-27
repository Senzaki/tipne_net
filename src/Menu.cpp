#include "Menu.hpp"
#include "Application.hpp"
#include "ResourceManager.hpp"
#include "Translator.hpp"

Menu::Menu(sf::RenderWindow &window, float vratio, float xyratio):
	m_window(window),
	m_camera(sf::FloatRect(0.f, 0.f, xyratio * DEFAULT_SCREEN_HEIGHT, DEFAULT_SCREEN_HEIGHT)),
	m_vratio(vratio),
	m_xyratio(xyratio)
{

}

Menu::~Menu()
{
	ResourceManager::getInstance().unloadSection(ResourceSection::Menu);
}

void Menu::load()
{
	ResourceManager &rmgr = ResourceManager::getInstance();
	Translator::getInstance().loadPackage("menu");
	//Load all textures & fonts
	rmgr.loadSection(ResourceSection::Menu);
	m_cursor.setTexture(rmgr.getTexture(ResourceSection::Base, "cursor.png"));
	//Create buttons
	m_buttons.reserve(4);
	m_buttons.emplace_back(tr("play"));
	m_buttons.emplace_back(tr("options"));
	m_buttons.emplace_back(tr("credits"));
	m_buttons.emplace_back(tr("quit"), std::bind(&Application::setNextAppState, &Application::getInstance(), nullptr, true));
	//Set the appropriate position
	float interval = (m_window.getSize().y - 100.f) / m_buttons.size();
	for(unsigned int i = 0; i < m_buttons.size(); i++)
		m_buttons[i].setPosition((m_window.getSize().x - m_buttons[i].getGlobalBounds().width) / 2.f, 100.f + i * interval);

}

void Menu::update(float etime)
{
}

void Menu::draw()
{
	m_window.clear();
	//Draw not scaled stuff
	for(Button &btn : m_buttons)
		btn.draw(m_window);
	//Apply the scaling view
	m_window.setView(m_camera);
	//Draw every scaled thing
	m_window.draw(m_cursor);
	//Back to default view
	m_window.setView(m_window.getDefaultView());
}

void Menu::onWindowClosed()
{
	Application::getInstance().setNextAppState(nullptr);
}

void Menu::onKeyPressed(const sf::Event::KeyEvent &evt)
{
	switch(evt.code)
	{
		case sf::Keyboard::Escape:
			Application::getInstance().setNextAppState(nullptr);
			break;

		default:
			break;
	}
}

void Menu::onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt)
{
	sf::Vector2f wincoords(evt.x, evt.y);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
	for(Button &btn : m_buttons)
		btn.onMouseButtonPressed(wincoords);
}

void Menu::onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt)
{
	sf::Vector2f wincoords(evt.x, evt.y);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
	for(Button &btn : m_buttons)
		btn.onMouseButtonReleased(wincoords);
}

void Menu::onMouseMoved(const sf::Event::MouseMoveEvent &evt)
{
	sf::Vector2f wincoords(evt.x, evt.y);
	sf::Vector2f camcoords(evt.x / m_vratio, evt.y / m_vratio);
	//Cursor is displayed in camera coords, but mouse coordinates are given in window coordinates
	m_cursor.setPosition(camcoords);
	//...but buttons are in window coords
	for(Button &btn : m_buttons)
		btn.onMouseMoved(wincoords);
}
