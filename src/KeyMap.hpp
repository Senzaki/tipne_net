#ifndef KEYMAP_HPP_INCLUDED
#define KEYMAP_HPP_INCLUDED

#include <SFML/Window.hpp>
#include <map>

enum class KeyAction
{
	None = -1,
	Left = 0,
	Right,
	Up,
	Down,
	Spell1,
	Count
};

class KeyMap
{
	public:
	static KeyMap &getInstance();

	KeyMap(const KeyMap &) = delete;
	KeyMap &operator=(const KeyMap &) = delete;

	void load();
	void save() const;
	void clear();

	KeyAction getActionForKey(const sf::Event::KeyEvent &event);
	void setActionForKey(const sf::Event::KeyEvent &event, KeyAction action);

	const char *getKeyName(const sf::Event::KeyEvent &event) const;
	const char *getActionName(KeyAction action) const;

	private:
	KeyMap();

	void initKeyNames();
	void initScanNames();
	void initActionNames();

	sf::Keyboard::Key internalStringToKey(const std::string &name) const;
	sf::Keyboard::ScanCode internalStringToScan(const std::string &name) const;
	const char *internalKeyToString(sf::Keyboard::Key key) const;
	const char *internalScanToString(sf::Keyboard::ScanCode scancode) const;
	KeyAction stringToAction(const std::string &name) const;

	KeyAction m_keymap[sf::Keyboard::KeyCount];
	KeyAction m_scanmap[sf::Keyboard::ScanCount];

	const char *m_keynames[sf::Keyboard::KeyCount];
	const char *m_scannames[sf::Keyboard::ScanCount];
	const char *m_actionnames[static_cast<int>(KeyAction::Count)];
	//Use maps, because this does not need to be very optimized, and because their memory footprint is lower than unordered_map
	std::map<std::string, sf::Keyboard::Key> m_keynameslookup;
	std::map<std::string, sf::Keyboard::ScanCode> m_scannameslookup;
	std::map<std::string, KeyAction> m_actionnameslookup;
};

#endif // KEYMAP_HPP_INCLUDED
