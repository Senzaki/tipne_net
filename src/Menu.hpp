#ifndef MENU_HPP_INCLUDED
#define MENU_HPP_INCLUDED

#include "ApplicationState.hpp"
#include "GUIManager.hpp"
#include "GameSimulator.hpp"
#include "DecoratedLineEdit.hpp"

class Menu : public ApplicationState
{
	public:
	Menu(sf::RenderWindow &window, float vratio, float xyratio);
	virtual ~Menu();

	Menu(const Menu &) = delete;
	Menu &operator=(const Menu &) = delete;

	virtual void load();
	virtual void update(float etime);
	virtual void draw();

	virtual void onWindowClosed();
	virtual void onKeyPressed(const sf::Event::KeyEvent &evt);
	virtual void onMouseButtonPressed(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseButtonReleased(const sf::Event::MouseButtonEvent &evt);
	virtual void onMouseMoved(const sf::Event::MouseMoveEvent &evt);
	virtual void onTextEntered(const sf::Event::TextEvent &evt);

	private:
	void showMainMenu();
	void showOptions();
	void showConnectMenu();
	void showHostMenu();
	void host(DecoratedLineEdit *tcpportwidget, DecoratedLineEdit *udpportwidget);
	void connect(DecoratedLineEdit *ipaddrwidget, DecoratedLineEdit *tcpportwidget, DecoratedLineEdit *udpportwidget);
	bool convertPorts(const std::string &tcpportstr, const std::string &udpportstr, unsigned short &tcpport, unsigned short &udpport);
	void launchGame(GameSimulator *simulator);

	sf::RenderWindow &m_window;
	GUIManager m_guimgr;
	sf::View m_camera;//View for the drawables that NEED TO BE SCALED (e.g. images), but not the other ones (e.g. fonts)
	float m_vratio;
	float m_xyratio;
	sf::Sprite m_cursor;
};

#endif // MENU_HPP_INCLUDED
