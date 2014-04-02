#include "KeyMap.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

constexpr const char *KEYS_FILENAME = "data/keys";
constexpr const char *UNKNOWN_KEY_NAME = "Unknown key";
constexpr const char *UNKNOWN_ACTION_NAME = "Unknown action";

KeyMap &KeyMap::getInstance()
{
	static KeyMap instance;
	return instance;
}

KeyMap::KeyMap():
	m_keynames{nullptr}, m_scannames{nullptr}, m_actionnames{nullptr}
{
	clear();
	initKeyNames();
	initScanNames();
	initActionNames();
	load();
}

static bool extractEnd(std::istringstream &strm, std::string &end)
{
	std::stringbuf temp;
	strm.get(temp);
	end = temp.str();
	return strm;
}

void KeyMap::load()
{
	std::ifstream file(KEYS_FILENAME);
	std::string line;
	unsigned int linenum = 0;
	while(std::getline(file, line))
	{
		linenum++;
		if(line.empty())
			continue;
		if(line[0] == '#')
			continue;
		if(line.substr(0, 4) == "key ")
		{
			std::istringstream strm(line.substr(4));
			std::string keyname;
			std::string actionname;
			if(!(strm >> actionname))
			{
				std::cerr << "Wrong line format in file " << KEYS_FILENAME << " at line " << linenum << "." << std::endl;
				continue;
			}
			strm.get();
			if(!extractEnd(strm, keyname))
			{
				std::cerr << "Wrong line format in file " << KEYS_FILENAME << " at line " << linenum << "." << std::endl;
				continue;
			}

			sf::Keyboard::Key key = internalStringToKey(keyname);
			if(key == sf::Keyboard::Unknown)
			{
				std::cerr << "Error in file " << KEYS_FILENAME << " at line " << linenum << " : unknown key name." << std::endl;
				continue;
			}
			KeyAction action = stringToAction(actionname);
			if(action == KeyAction::None)
			{
				std::cerr << "Error in file " << KEYS_FILENAME << " at line " << linenum << " : unknown action name." << std::endl;
				continue;
			}
			m_keymap[key] = action;
		}
		else if(line.substr(0, 5) == "scan ")
		{
			std::istringstream strm(line.substr(5));
			std::string scanname;
			std::string actionname;
			if(!(strm >> actionname))
			{
				std::cerr << "Wrong line format in file " << KEYS_FILENAME << " at line " << linenum << "." << std::endl;
				continue;
			}
			strm.get();
			if(!extractEnd(strm, scanname))
			{
				std::cerr << "Wrong line format in file " << KEYS_FILENAME << " at line " << linenum << "." << std::endl;
				continue;
			}

			sf::Keyboard::ScanCode scancode = internalStringToScan(scanname);
			if(scancode == sf::Keyboard::ScanUnknown)
			{
				std::cerr << "Error in file " << KEYS_FILENAME << " at line " << linenum << " : unknown scancode name." << std::endl;
				continue;
			}
			KeyAction action = stringToAction(actionname);
			if(action == KeyAction::None)
			{
				std::cerr << "Error in file " << KEYS_FILENAME << " at line " << linenum << " : unknown action name." << std::endl;
				continue;
			}
			m_scanmap[scancode] = action;
		}
		else
			std::cerr << "Wrong line format in file " << KEYS_FILENAME << " at line " << linenum << "." << std::endl;
	}
	file.close();
}

void KeyMap::save() const
{
	std::ofstream file(KEYS_FILENAME);
	if(!file)
	{
		std::cerr << "Cannot create/write to file " << KEYS_FILENAME << ". Key mapping will not be saved." << std::endl;
		return;
	}
	for(unsigned int i = 0; i < sf::Keyboard::KeyCount; i++)
	{
		if(m_keymap[i] != KeyAction::None)
			file << "key " << getActionName(m_keymap[i]) << " " << internalKeyToString(static_cast<sf::Keyboard::Key>(i)) << std::endl;
	}
	for(unsigned int i = 0; i < sf::Keyboard::ScanCount; i++)
	{
		if(m_scanmap[i] != KeyAction::None)
			file << "scan " << getActionName(m_scanmap[i]) << " " << internalScanToString(static_cast<sf::Keyboard::ScanCode>(i)) << std::endl;
	}
	file.close();
}

void KeyMap::clear()
{
	for(unsigned int i = 0; i < sf::Keyboard::KeyCount; i++)
		m_keymap[i] = KeyAction::None;
	for(unsigned int i = 0; i < sf::Keyboard::ScanCount; i++)
		m_scanmap[i] = KeyAction::None;
}

void KeyMap::setActionForKey(const sf::Event::KeyEvent &event, KeyAction action)
{
	if(action == KeyAction::Count)
		return;
	if(event.code != sf::Keyboard::Unknown && event.code < sf::Keyboard::KeyCount)
		m_keymap[event.code] = action;
	if(event.scancode != sf::Keyboard::ScanUnknown && event.scancode < sf::Keyboard::ScanCount)
		m_scanmap[event.scancode] = action;
}

KeyAction KeyMap::getActionForKey(const sf::Event::KeyEvent &event)
{
	//Priority : "virtual" key codes
	if(event.code != sf::Keyboard::Unknown)
	{
		//If an action is bound to this key, return it
		if(m_keymap[event.code] != KeyAction::None)
			return m_keymap[event.code];
	}
	//If no "virtual" key code, use scan codes
	if(event.scancode != sf::Keyboard::ScanUnknown)
		return m_scanmap[event.scancode];
	//Unrecognized key or no action bound
	return KeyAction::None;
}

const char *KeyMap::getKeyName(const sf::Event::KeyEvent &event) const
{
	const char *name;
	//Priority : "virtual" key codes
	if((name = internalKeyToString(event.code)) == nullptr)
	{
		if((name = internalScanToString(event.scancode)) == nullptr)
			return UNKNOWN_KEY_NAME;
	}
	return name;
}

const char *KeyMap::getActionName(KeyAction action) const
{
	if(action == KeyAction::None || action == KeyAction::Count)
		return UNKNOWN_ACTION_NAME;
	const char *name = m_actionnames[static_cast<int>(action)];
	if(name == nullptr)
		return UNKNOWN_ACTION_NAME;
	return name;
}

void KeyMap::initKeyNames()
{
	m_keynames[sf::Keyboard::A] = "A";
	m_keynames[sf::Keyboard::B] = "B";
	m_keynames[sf::Keyboard::C] = "C";
	m_keynames[sf::Keyboard::D] = "D";
	m_keynames[sf::Keyboard::E] = "E";
	m_keynames[sf::Keyboard::F] = "F";
	m_keynames[sf::Keyboard::G] = "G";
	m_keynames[sf::Keyboard::H] = "H";
	m_keynames[sf::Keyboard::I] = "I";
	m_keynames[sf::Keyboard::J] = "J";
	m_keynames[sf::Keyboard::K] = "K";
	m_keynames[sf::Keyboard::L] = "L";
	m_keynames[sf::Keyboard::M] = "M";
	m_keynames[sf::Keyboard::N] = "N";
	m_keynames[sf::Keyboard::O] = "O";
	m_keynames[sf::Keyboard::P] = "P";
	m_keynames[sf::Keyboard::Q] = "Q";
	m_keynames[sf::Keyboard::R] = "R";
	m_keynames[sf::Keyboard::S] = "S";
	m_keynames[sf::Keyboard::T] = "T";
	m_keynames[sf::Keyboard::U] = "U";
	m_keynames[sf::Keyboard::V] = "V";
	m_keynames[sf::Keyboard::W] = "W";
	m_keynames[sf::Keyboard::X] = "X";
	m_keynames[sf::Keyboard::Y] = "Y";
	m_keynames[sf::Keyboard::Z] = "Z";
	m_keynames[sf::Keyboard::Num0] = "0";
	m_keynames[sf::Keyboard::Num1] = "1";
	m_keynames[sf::Keyboard::Num2] = "2";
	m_keynames[sf::Keyboard::Num3] = "3";
	m_keynames[sf::Keyboard::Num4] = "4";
	m_keynames[sf::Keyboard::Num5] = "5";
	m_keynames[sf::Keyboard::Num6] = "6";
	m_keynames[sf::Keyboard::Num7] = "7";
	m_keynames[sf::Keyboard::Num8] = "8";
	m_keynames[sf::Keyboard::Num9] = "9";
	m_keynames[sf::Keyboard::Escape] = "Escape";
	m_keynames[sf::Keyboard::LControl] = "Left Control";
	m_keynames[sf::Keyboard::LShift] = "Left Shift";
	m_keynames[sf::Keyboard::LAlt] = "Left Alt";
	m_keynames[sf::Keyboard::LSystem] = "Left System";
	m_keynames[sf::Keyboard::RControl] = "Right Control";
	m_keynames[sf::Keyboard::RShift] = "Right Shift";
	m_keynames[sf::Keyboard::RAlt] = "Right Alt";
	m_keynames[sf::Keyboard::RSystem] = "Right System";
	m_keynames[sf::Keyboard::Menu] = "Menu";
	m_keynames[sf::Keyboard::LBracket] = "Left Bracket";
	m_keynames[sf::Keyboard::RBracket] = "Right Bracket";
	m_keynames[sf::Keyboard::SemiColon] = "Semicolon";
	m_keynames[sf::Keyboard::Comma] = ",";
	m_keynames[sf::Keyboard::Period] = ".";
	m_keynames[sf::Keyboard::Quote] = "\"";
	m_keynames[sf::Keyboard::Slash] = "/";
	m_keynames[sf::Keyboard::BackSlash] = "\\";
	m_keynames[sf::Keyboard::Tilde] = "~";
	m_keynames[sf::Keyboard::Equal] = "=";
	m_keynames[sf::Keyboard::Dash] = "-";
	m_keynames[sf::Keyboard::Space] = "Space";
	m_keynames[sf::Keyboard::Return] = "Return";
	m_keynames[sf::Keyboard::BackSpace] = "Backspace";
	m_keynames[sf::Keyboard::Tab] = "Tab";
	m_keynames[sf::Keyboard::PageUp] = "Page Up";
	m_keynames[sf::Keyboard::PageDown] = "Page Down";
	m_keynames[sf::Keyboard::End] = "End";
	m_keynames[sf::Keyboard::Home] = "Home";
	m_keynames[sf::Keyboard::Insert] = "Insert";
	m_keynames[sf::Keyboard::Delete] = "Delete";
	m_keynames[sf::Keyboard::Add] = "Numpad +";
	m_keynames[sf::Keyboard::Subtract] = "Numpad -";
	m_keynames[sf::Keyboard::Multiply] = "Numpad *";
	m_keynames[sf::Keyboard::Divide] = "Numpad /";
	m_keynames[sf::Keyboard::Left] = "Left";
	m_keynames[sf::Keyboard::Right] = "Right";
	m_keynames[sf::Keyboard::Up] = "Up";
	m_keynames[sf::Keyboard::Down] = "Down";
	m_keynames[sf::Keyboard::Numpad0] = "Numpad 0";
	m_keynames[sf::Keyboard::Numpad1] = "Numpad 1";
	m_keynames[sf::Keyboard::Numpad2] = "Numpad 2";
	m_keynames[sf::Keyboard::Numpad3] = "Numpad 3";
	m_keynames[sf::Keyboard::Numpad4] = "Numpad 4";
	m_keynames[sf::Keyboard::Numpad5] = "Numpad 5";
	m_keynames[sf::Keyboard::Numpad6] = "Numpad 6";
	m_keynames[sf::Keyboard::Numpad7] = "Numpad 7";
	m_keynames[sf::Keyboard::Numpad8] = "Numpad 8";
	m_keynames[sf::Keyboard::Numpad9] = "Numpad 9";
	m_keynames[sf::Keyboard::F1] = "F1";
	m_keynames[sf::Keyboard::F2] = "F2";
	m_keynames[sf::Keyboard::F3] = "F3";
	m_keynames[sf::Keyboard::F4] = "F4";
	m_keynames[sf::Keyboard::F5] = "F5";
	m_keynames[sf::Keyboard::F6] = "F6";
	m_keynames[sf::Keyboard::F7] = "F7";
	m_keynames[sf::Keyboard::F8] = "F8";
	m_keynames[sf::Keyboard::F9] = "F9";
	m_keynames[sf::Keyboard::F10] = "F10";
	m_keynames[sf::Keyboard::F11] = "F11";
	m_keynames[sf::Keyboard::F12] = "F12";
	m_keynames[sf::Keyboard::F13] = "F13";
	m_keynames[sf::Keyboard::F14] = "F14";
	m_keynames[sf::Keyboard::F15] = "F15";
	m_keynames[sf::Keyboard::Pause] = "Pause";
	//Create reverse lookup table
	for(unsigned int i = 0; i < sf::Keyboard::KeyCount; i++)
	{
		//If the string is defined
		if(m_keynames[i] != nullptr)
		{
			//If a key already has this name, print a warning
			if(!m_keynameslookup.emplace(m_keynames[i], static_cast<sf::Keyboard::Key>(i)).second)
				std::cerr << "Warning : duplicate key name \"" << m_keynames[i] << "\". The duplicate key may be ignored." << std::endl;
		}
	}
}

void KeyMap::initScanNames()
{
	m_scannames[sf::Keyboard::ScanA] = "A";
	m_scannames[sf::Keyboard::ScanB] = "B";
	m_scannames[sf::Keyboard::ScanC] = "C";
	m_scannames[sf::Keyboard::ScanD] = "D";
	m_scannames[sf::Keyboard::ScanE] = "E";
	m_scannames[sf::Keyboard::ScanF] = "F";
	m_scannames[sf::Keyboard::ScanG] = "G";
	m_scannames[sf::Keyboard::ScanH] = "H";
	m_scannames[sf::Keyboard::ScanI] = "I";
	m_scannames[sf::Keyboard::ScanJ] = "J";
	m_scannames[sf::Keyboard::ScanK] = "K";
	m_scannames[sf::Keyboard::ScanL] = "L";
	m_scannames[sf::Keyboard::ScanM] = "M";
	m_scannames[sf::Keyboard::ScanN] = "N";
	m_scannames[sf::Keyboard::ScanO] = "O";
	m_scannames[sf::Keyboard::ScanP] = "P";
	m_scannames[sf::Keyboard::ScanQ] = "Q";
	m_scannames[sf::Keyboard::ScanR] = "R";
	m_scannames[sf::Keyboard::ScanS] = "S";
	m_scannames[sf::Keyboard::ScanT] = "T";
	m_scannames[sf::Keyboard::ScanU] = "U";
	m_scannames[sf::Keyboard::ScanV] = "V";
	m_scannames[sf::Keyboard::ScanW] = "W";
	m_scannames[sf::Keyboard::ScanX] = "X";
	m_scannames[sf::Keyboard::ScanY] = "Y";
	m_scannames[sf::Keyboard::ScanZ] = "Z";
	m_scannames[sf::Keyboard::Scan1] = "1";
	m_scannames[sf::Keyboard::Scan2] = "2";
	m_scannames[sf::Keyboard::Scan3] = "3";
	m_scannames[sf::Keyboard::Scan4] = "4";
	m_scannames[sf::Keyboard::Scan5] = "5";
	m_scannames[sf::Keyboard::Scan6] = "6";
	m_scannames[sf::Keyboard::Scan7] = "7";
	m_scannames[sf::Keyboard::Scan8] = "8";
	m_scannames[sf::Keyboard::Scan9] = "9";
	m_scannames[sf::Keyboard::Scan0] = "0";
	m_scannames[sf::Keyboard::ScanReturn] = "Return";
	m_scannames[sf::Keyboard::ScanEscape] = "Escape";
	m_scannames[sf::Keyboard::ScanBackspace] = "Backspace";
	m_scannames[sf::Keyboard::ScanTab] = "Tab";
	m_scannames[sf::Keyboard::ScanSpace] = "Space";
	m_scannames[sf::Keyboard::ScanMinus] = "-";
	m_scannames[sf::Keyboard::ScanEquals] = "=";
	m_scannames[sf::Keyboard::ScanLeftBracket] = "[";
	m_scannames[sf::Keyboard::ScanRightBracket] = "]";
	m_scannames[sf::Keyboard::ScanBackSlash] = "\\";
	m_scannames[sf::Keyboard::ScanNonUSHash] = "Non US #";
	m_scannames[sf::Keyboard::ScanSemicolon] = ";";
	m_scannames[sf::Keyboard::ScanApostrophe] = "'";
	m_scannames[sf::Keyboard::ScanGrave] = "Grave";
	m_scannames[sf::Keyboard::ScanComma] = ",";
	m_scannames[sf::Keyboard::ScanPeriod] = ".";
	m_scannames[sf::Keyboard::ScanSlash] = "/";
	m_scannames[sf::Keyboard::ScanCapsLock] = "Caps Lock";
	m_scannames[sf::Keyboard::ScanF1] = "F1";
	m_scannames[sf::Keyboard::ScanF2] = "F2";
	m_scannames[sf::Keyboard::ScanF3] = "F3";
	m_scannames[sf::Keyboard::ScanF4] = "F4";
	m_scannames[sf::Keyboard::ScanF5] = "F5";
	m_scannames[sf::Keyboard::ScanF6] = "F6";
	m_scannames[sf::Keyboard::ScanF7] = "F7";
	m_scannames[sf::Keyboard::ScanF8] = "F8";
	m_scannames[sf::Keyboard::ScanF9] = "F9";
	m_scannames[sf::Keyboard::ScanF10] = "F10";
	m_scannames[sf::Keyboard::ScanF11] = "F11";
	m_scannames[sf::Keyboard::ScanF12] = "F12";
	m_scannames[sf::Keyboard::ScanPrintScreen] = "Print Screen";
	m_scannames[sf::Keyboard::ScanScrollLock] = "Scroll Lock";
	m_scannames[sf::Keyboard::ScanPause] = "Pause";
	m_scannames[sf::Keyboard::ScanInsert] = "Insert";
	m_scannames[sf::Keyboard::ScanHome] = "Home";
	m_scannames[sf::Keyboard::ScanPageUp] = "Page Up";
	m_scannames[sf::Keyboard::ScanDelete] = "Delete";
	m_scannames[sf::Keyboard::ScanEnd] = "End";
	m_scannames[sf::Keyboard::ScanPageDown] = "Page Down";
	m_scannames[sf::Keyboard::ScanRight] = "Right";
	m_scannames[sf::Keyboard::ScanLeft] = "Left";
	m_scannames[sf::Keyboard::ScanDown] = "Down";
	m_scannames[sf::Keyboard::ScanUp] = "Up";
	m_scannames[sf::Keyboard::ScanNumLockClear] = "Num Lock";
	m_scannames[sf::Keyboard::ScanNumpadDivide] = "Numpad /";
	m_scannames[sf::Keyboard::ScanNumpadMultiply] = "Numpad *";
	m_scannames[sf::Keyboard::ScanNumpadMinus] = "Numpad -";
	m_scannames[sf::Keyboard::ScanNumpadPlus] = "Numpad +";
	m_scannames[sf::Keyboard::ScanNumpadEnter] = "Numpad Enter";
	m_scannames[sf::Keyboard::ScanNumpad1] = "Numpad 1";
	m_scannames[sf::Keyboard::ScanNumpad2] = "Numpad 2";
	m_scannames[sf::Keyboard::ScanNumpad3] = "Numpad 3";
	m_scannames[sf::Keyboard::ScanNumpad4] = "Numpad 4";
	m_scannames[sf::Keyboard::ScanNumpad5] = "Numpad 5";
	m_scannames[sf::Keyboard::ScanNumpad6] = "Numpad 6";
	m_scannames[sf::Keyboard::ScanNumpad7] = "Numpad 7";
	m_scannames[sf::Keyboard::ScanNumpad8] = "Numpad 8";
	m_scannames[sf::Keyboard::ScanNumpad9] = "Numpad 9";
	m_scannames[sf::Keyboard::ScanNumpad0] = "Numpad 0";
	m_scannames[sf::Keyboard::ScanNumpadPeriod] = "Numpad .";
	m_scannames[sf::Keyboard::ScanNonUSBackSlash] = "Non US \\";
	m_scannames[sf::Keyboard::ScanApplication] = "Application";
	m_scannames[sf::Keyboard::ScanPower] = "Power";
	m_scannames[sf::Keyboard::ScanNumpadEquals] = "Numpad =";
	m_scannames[sf::Keyboard::ScanF13] = "F13";
	m_scannames[sf::Keyboard::ScanF14] = "F14";
	m_scannames[sf::Keyboard::ScanF15] = "F15";
	m_scannames[sf::Keyboard::ScanF16] = "F16";
	m_scannames[sf::Keyboard::ScanF17] = "F17";
	m_scannames[sf::Keyboard::ScanF18] = "F18";
	m_scannames[sf::Keyboard::ScanF19] = "F19";
	m_scannames[sf::Keyboard::ScanF20] = "F20";
	m_scannames[sf::Keyboard::ScanF21] = "F21";
	m_scannames[sf::Keyboard::ScanF22] = "F22";
	m_scannames[sf::Keyboard::ScanF23] = "F23";
	m_scannames[sf::Keyboard::ScanF24] = "F24";
	m_scannames[sf::Keyboard::ScanExecute] = "Execute";
	m_scannames[sf::Keyboard::ScanHelp] = "Help";
	m_scannames[sf::Keyboard::ScanMenu] = "Menu";
	m_scannames[sf::Keyboard::ScanSelect] = "Select";
	m_scannames[sf::Keyboard::ScanStop] = "Stop";
	m_scannames[sf::Keyboard::ScanAgain] = "Again";
	m_scannames[sf::Keyboard::ScanUndo] = "Undo";
	m_scannames[sf::Keyboard::ScanCut] = "Cut";
	m_scannames[sf::Keyboard::ScanCopy] = "Copy";
	m_scannames[sf::Keyboard::ScanPaste] = "Paste";
	m_scannames[sf::Keyboard::ScanFind] = "Find";
	m_scannames[sf::Keyboard::ScanMute] = "Mute";
	m_scannames[sf::Keyboard::ScanVolumeUp] = "Volume Up";
	m_scannames[sf::Keyboard::ScanVolumeDown] = "Volume Down";
	m_scannames[sf::Keyboard::ScanNumpadComma] = "Numpad ,";
	m_scannames[sf::Keyboard::ScanNumpadEqualsAS400] = "Numpad = AS400";
	m_scannames[sf::Keyboard::ScanInternational1] = "International 1";
	m_scannames[sf::Keyboard::ScanInternational2] = "International 2";
	m_scannames[sf::Keyboard::ScanInternational3] = "International 3";
	m_scannames[sf::Keyboard::ScanInternational4] = "International 4";
	m_scannames[sf::Keyboard::ScanInternational5] = "International 5";
	m_scannames[sf::Keyboard::ScanInternational6] = "International 6";
	m_scannames[sf::Keyboard::ScanInternational7] = "International 7";
	m_scannames[sf::Keyboard::ScanInternational8] = "International 8";
	m_scannames[sf::Keyboard::ScanInternational9] = "International 9";
	m_scannames[sf::Keyboard::ScanLang1] = "Lang 1";
	m_scannames[sf::Keyboard::ScanLang2] = "Lang 2";
	m_scannames[sf::Keyboard::ScanLang3] = "Lang 3";
	m_scannames[sf::Keyboard::ScanLang4] = "Lang 4";
	m_scannames[sf::Keyboard::ScanLang5] = "Lang 5";
	m_scannames[sf::Keyboard::ScanLang6] = "Lang 6";
	m_scannames[sf::Keyboard::ScanLang7] = "Lang 7";
	m_scannames[sf::Keyboard::ScanLang8] = "Lang 8";
	m_scannames[sf::Keyboard::ScanLang9] = "Lang 9";
	m_scannames[sf::Keyboard::ScanAltErase] = "Alt Erase";
	m_scannames[sf::Keyboard::ScanSysreq] = "Sysreq";
	m_scannames[sf::Keyboard::ScanCancel] = "Cancel";
	m_scannames[sf::Keyboard::ScanClear] = "Clear";
	m_scannames[sf::Keyboard::ScanPrior] = "Prior";
	m_scannames[sf::Keyboard::ScanReturn2] = "Return 2";
	m_scannames[sf::Keyboard::ScanSeparator] = "Separator";
	m_scannames[sf::Keyboard::ScanOut] = "Out";
	m_scannames[sf::Keyboard::ScanOper] = "Oper";
	m_scannames[sf::Keyboard::ScanClearAgain] = "Clear Again";
	m_scannames[sf::Keyboard::ScanCrSel] = "CrSel";
	m_scannames[sf::Keyboard::ScanExSel] = "ExSel";
	m_scannames[sf::Keyboard::ScanNumpad00] = "Numpad 00";
	m_scannames[sf::Keyboard::ScanNumpad000] = "Numpad 000";
	m_scannames[sf::Keyboard::ScanThousandsSeparator] = "Thousands Separator";
	m_scannames[sf::Keyboard::ScanDecimalSeparator] = "Decimal Separator";
	m_scannames[sf::Keyboard::ScanCurrencyUnit] = "Currency Unit";
	m_scannames[sf::Keyboard::ScanCurrencySubunit] = "Currency Subunit";
	m_scannames[sf::Keyboard::ScanNumpadLeftParenthesis] = "Numpad Left Parenthesis";
	m_scannames[sf::Keyboard::ScanNumpadRightParenthesis] = "Numpad Right Parenthesis";
	m_scannames[sf::Keyboard::ScanNumpadLeftBrace] = "Numpad Left Brace";
	m_scannames[sf::Keyboard::ScanNumpadRightBrace] = "Numpad Right Brace";
	m_scannames[sf::Keyboard::ScanNumpadTab] = "Numpad Tab";
	m_scannames[sf::Keyboard::ScanNumpadBackspace] = "Numpad Backspace";
	m_scannames[sf::Keyboard::ScanNumpadA] = "Numpad A";
	m_scannames[sf::Keyboard::ScanNumpadB] = "Numpad B";
	m_scannames[sf::Keyboard::ScanNumpadC] = "Numpad C";
	m_scannames[sf::Keyboard::ScanNumpadD] = "Numpad D";
	m_scannames[sf::Keyboard::ScanNumpadE] = "Numpad E";
	m_scannames[sf::Keyboard::ScanNumpadF] = "Numpad F";
	m_scannames[sf::Keyboard::ScanNumpadXor] = "Numpad Xor";
	m_scannames[sf::Keyboard::ScanNumpadPower] = "Numpad Power";
	m_scannames[sf::Keyboard::ScanNumpadPercent] = "Numpad %";
	m_scannames[sf::Keyboard::ScanNumpadLess] = "Numpad <";
	m_scannames[sf::Keyboard::ScanNumpadGreater] = "Numpad >";
	m_scannames[sf::Keyboard::ScanNumpadAmpersand] = "Numpad &";
	m_scannames[sf::Keyboard::ScanNumpadDoubleAmpersand] = "Numpad &&";
	m_scannames[sf::Keyboard::ScanNumpadVerticalBar] = "Numpad |";
	m_scannames[sf::Keyboard::ScanNumpadDoubleVerticalBar] = "Numpad ||";
	m_scannames[sf::Keyboard::ScanNumpadColon] = "Numpad :";
	m_scannames[sf::Keyboard::ScanNumpadHash] = "Numpad #";
	m_scannames[sf::Keyboard::ScanNumpadSpace] = "Numpad Space";
	m_scannames[sf::Keyboard::ScanNumpadAt] = "Numpad @";
	m_scannames[sf::Keyboard::ScanNumpadExclam] = "Numpad !";
	m_scannames[sf::Keyboard::ScanNumpadMemStore] = "Numpad Mem Store";
	m_scannames[sf::Keyboard::ScanNumpadMemRecall] = "Numpad Mem Recall";
	m_scannames[sf::Keyboard::ScanNumpadMemClear] = "Numpad Mem Clear";
	m_scannames[sf::Keyboard::ScanNumpadMemAdd] = "Numpad Mem +";
	m_scannames[sf::Keyboard::ScanNumpadMemSubtract] = "Numpad Mem -";
	m_scannames[sf::Keyboard::ScanNumpadMemMultiply] = "Numpad Mem *";
	m_scannames[sf::Keyboard::ScanNumpadMemDivide] = "Numpad Mem /";
	m_scannames[sf::Keyboard::ScanNumpadPlusMinus] = "Numpad +/-";
	m_scannames[sf::Keyboard::ScanNumpadClear] = "Numpad Clear";
	m_scannames[sf::Keyboard::ScanNumpadClearEntry] = "Numpad Clear Entry";
	m_scannames[sf::Keyboard::ScanNumpadBinary] = "Numpad Binary";
	m_scannames[sf::Keyboard::ScanNumpadOctal] = "Numpad Octal";
	m_scannames[sf::Keyboard::ScanNumpadDecimal] = "Numpad Decimal";
	m_scannames[sf::Keyboard::ScanNumpadHexadecimal] = "Numpad Hexadecimal";
	m_scannames[sf::Keyboard::ScanLCtrl] = "Left Ctrl";
	m_scannames[sf::Keyboard::ScanLShift] = "Left Shift";
	m_scannames[sf::Keyboard::ScanLAlt] = "Left Alt";
	m_scannames[sf::Keyboard::ScanLGui] = "Left Gui";
	m_scannames[sf::Keyboard::ScanRCtrl] = "Right Ctrl";
	m_scannames[sf::Keyboard::ScanRShift] = "Right Shift";
	m_scannames[sf::Keyboard::ScanRAlt] = "Right Alt";
	m_scannames[sf::Keyboard::ScanRGui] = "Right Gui";
	m_scannames[sf::Keyboard::ScanMode] = "Mode";
	m_scannames[sf::Keyboard::ScanAudioNext] = "Audio Next";
	m_scannames[sf::Keyboard::ScanAudioPrev] = "Audio Prev";
	m_scannames[sf::Keyboard::ScanAudioStop] = "Audio Stop";
	m_scannames[sf::Keyboard::ScanAudioPlay] = "Audio Play";
	m_scannames[sf::Keyboard::ScanAudioMute] = "Audio Mute";
	m_scannames[sf::Keyboard::ScanMediaSelect] = "Media Select";
	m_scannames[sf::Keyboard::ScanWww] = "WWW";
	m_scannames[sf::Keyboard::ScanMail] = "Mail";
	m_scannames[sf::Keyboard::ScanCalculator] = "Calculator";
	m_scannames[sf::Keyboard::ScanComputer] = "Computer";
	m_scannames[sf::Keyboard::ScanAcSearch] = "Ac Search";
	m_scannames[sf::Keyboard::ScanAcHome] = "Ac Home";
	m_scannames[sf::Keyboard::ScanAcBack] = "Ac Back";
	m_scannames[sf::Keyboard::ScanAcForward] = "Ac Forward";
	m_scannames[sf::Keyboard::ScanAcStop] = "Ac Stop";
	m_scannames[sf::Keyboard::ScanAcRefresh] = "Ac Refresh";
	m_scannames[sf::Keyboard::ScanAcBookmarks] = "Ac Bookmarks";
	m_scannames[sf::Keyboard::ScanBrightnessDown] = "Brightness Down";
	m_scannames[sf::Keyboard::ScanBrightnessUp] = "Brightness Up";
	m_scannames[sf::Keyboard::ScanDisplaySwitch] = "Display Switch";
	m_scannames[sf::Keyboard::ScanIllumToggle] = "Illum Toggle";
	m_scannames[sf::Keyboard::ScanIllumDown] = "Illum Down";
	m_scannames[sf::Keyboard::ScanIllumUp] = "Illum Up";
	m_scannames[sf::Keyboard::ScanEject] = "Eject";
	m_scannames[sf::Keyboard::ScanSleep] = "Sleep";
	m_scannames[sf::Keyboard::ScanApp1] = "App 1";
	m_scannames[sf::Keyboard::ScanApp2] = "App 2";

	//Create reverse lookup table
	for(unsigned int i = 0; i < sf::Keyboard::ScanCount; i++)
	{
		//If the string is defined
		if(m_scannames[i] != nullptr)
		{
			//If a scan code already has this name, print a warning
			if(!m_scannameslookup.emplace(m_scannames[i], static_cast<sf::Keyboard::ScanCode>(i)).second)
				std::cerr << "Warning : duplicate scan code name \"" << m_scannames[i] << "\". The duplicate key may be ignored." << std::endl;
		}
	}
}

void KeyMap::initActionNames()
{
	//Key action names MUST NOT contain any space
	//Use the "action_" prefix for translation packs convenience
	m_actionnames[(int)KeyAction::Left] = "action_left";
	m_actionnames[(int)KeyAction::Right] = "action_right";
	m_actionnames[(int)KeyAction::Up] = "action_up";
	m_actionnames[(int)KeyAction::Down] = "action_down";

	//Create reverse lookup table
	for(unsigned int i = 0; i < (unsigned int)KeyAction::Count; i++)
	{
		//If the string is defined
		if(m_actionnames[i] != nullptr)
		{
			//If a scan code already has this name, print a warning
			if(!m_actionnameslookup.emplace(m_actionnames[i], static_cast<KeyAction>(i)).second)
				std::cerr << "Warning : duplicate action name \"" << m_actionnames[i] << "\". The duplicate key may be ignored." << std::endl;
		}
	}
}

sf::Keyboard::Key KeyMap::internalStringToKey(const std::string &name) const
{
	//Check that the key name exists
	auto it = m_keynameslookup.find(name);
	if(it != m_keynameslookup.end())
		return it->second;
	return sf::Keyboard::Unknown;
}

sf::Keyboard::ScanCode KeyMap::internalStringToScan(const std::string &name) const
{
	//Check that the scancode name exists
	auto it = m_scannameslookup.find(name);
	if(it != m_scannameslookup.end())
		return it->second;
	return sf::Keyboard::ScanUnknown;
}

const char *KeyMap::internalKeyToString(sf::Keyboard::Key key) const
{
	if(key == sf::Keyboard::Unknown || key == sf::Keyboard::KeyCount)
		return nullptr;
	return m_keynames[key];
}

const char *KeyMap::internalScanToString(sf::Keyboard::ScanCode scancode) const
{
	if(scancode == sf::Keyboard::ScanUnknown || scancode == sf::Keyboard::ScanCount)
		return nullptr;
	return m_scannames[scancode];
}

KeyAction KeyMap::stringToAction(const std::string &name) const
{
	//Check that the action name exists
	auto it = m_actionnameslookup.find(name);
	if(it != m_actionnameslookup.end())
		return it->second;
	return KeyAction::None;
}
