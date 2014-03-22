#ifndef OPTIONSMENU_HPP_INCLUDED
#define OPTIONSMENU_HPP_INCLUDED

#include "Widget.hpp"
#include "Label.hpp"
#include "DecoratedLineEdit.hpp"
#include "CheckBox.hpp"
#include "Button.hpp"

enum OPTIONS_LABELS
{
	OPTIONS_NAME = 0,
	OPTIONS_VIDEOMODE,
	OPTIONS_VSYNC,
	OPTIONS_FULLSCREEN,
	OPTIONS_LANG,
	OPTIONS_LABEL_COUNT
};

class OptionsMenu : public Widget
{
	public:
	OptionsMenu(Widget *parent = nullptr, std::function<void()> callbacksave = std::function<void()>(), std::function<void()> callbackcancel = std::function<void()>());
	virtual ~OptionsMenu();
	virtual void draw(sf::RenderWindow &window) { }

	protected:
	virtual bool onKeyPressed(const sf::Event::KeyEvent &evt);

	private:
	void setWidgetsPosition();
	void setDefaultValues();
	void saveOptions();
	std::function<void()> m_callbacksave;
	Label *m_labels[OPTIONS_LABEL_COUNT];
	DecoratedLineEdit *m_name;
	CheckBox *m_vsync;
	CheckBox *m_fullscreen;
	Label *m_videomode;
	Label *m_lang;
	Button *m_videomodebuttons[2];
	Button *m_langbuttons[2];
	Button *m_cancel;
	Button *m_save;
};

#endif //OPTIONS_HPP_INCLUDED
