#pragma once
#ifndef ES_APP_GUIS_GUI_MENU_H
#define ES_APP_GUIS_GUI_MENU_H

#include "GuiComponent.h"
#include "components/MenuComponent.h"
#include <functional>

class GuiMenu : public GuiComponent
{
public:
	GuiMenu(Window* window);

	bool input(InputConfig* config, Input input) override;
	void onSizeChanged() override;
	std::vector<HelpPrompt> getHelpPrompts() override;
	HelpStyle getHelpStyle() override;

private:
	void addEntry(const char* name, unsigned int color, bool add_arrow, const std::function<void()>& func);
	void addVersionInfo();
	void openConfigInput();
	void openQuitMenu();
	void openSoundSettings();
	void openUISettings();

	MenuComponent mMenu;
	TextComponent mVersion;
};

#endif // ES_APP_GUIS_GUI_MENU_H
