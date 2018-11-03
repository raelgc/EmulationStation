#include "EmulationStation.h"
#include "guis/GuiMenu.h"
#include "Window.h"
#include "Sound.h"
#include "Log.h"
#include "Settings.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiSettings.h"
#include "guis/GuiDetectDevice.h"
#include "views/ViewController.h"

#include "components/ButtonComponent.h"
#include "components/SwitchComponent.h"
#include "components/SliderComponent.h"
#include "components/TextComponent.h"
#include "components/OptionListComponent.h"
#include "components/MenuComponent.h"
#include "VolumeControl.h"

GuiMenu::GuiMenu(Window* window) : GuiComponent(window), mMenu(window, "MAIN MENU"), mVersion(window)
{
	// MAIN MENU

	// SOUND SETTINGS >
	// UI SETTINGS >
	// CONFIGURE INPUT >
	// QUIT >

	// [version]

	if(!Settings::getInstance()->getBool("ForceKiosk"))
	{

		addEntry("UI SETTINGS", 0x777777FF, true, [this] {
			auto s = new GuiSettings(mWindow, "UI SETTINGS");
			// screensaver time
			auto screensaver_time = std::make_shared<SliderComponent>(mWindow, 0.f, 30.f, 1.f, "m");
			screensaver_time->setValue((float)(Settings::getInstance()->getInt("ScreenSaverTime") / (1000 * 60)));
			s->addWithLabel("SCREENSAVER AFTER", screensaver_time);
			s->addSaveFunc([screensaver_time] { Settings::getInstance()->setInt("ScreenSaverTime", (int)round(screensaver_time->getValue()) * (1000 * 60)); });
			// screensaver behavior
			auto screensaver_behavior = std::make_shared< OptionListComponent<std::string> >(mWindow, "TRANSITION STYLE", false);
			std::vector<std::string> screensavers;
			screensavers.push_back("dim");
			screensavers.push_back("black");
			for(auto it = screensavers.cbegin(); it != screensavers.cend(); it++)
				screensaver_behavior->add(*it, *it, Settings::getInstance()->getString("ScreenSaverBehavior") == *it);
			s->addWithLabel("SCREENSAVER BEHAVIOR", screensaver_behavior);
			s->addSaveFunc([screensaver_behavior] { Settings::getInstance()->setString("ScreenSaverBehavior", screensaver_behavior->getSelected()); });
			// framerate
			auto framerate = std::make_shared<SwitchComponent>(mWindow);
			framerate->setState(Settings::getInstance()->getBool("DrawFramerate"));
			s->addWithLabel("SHOW FRAMERATE", framerate);
			s->addSaveFunc([framerate] { Settings::getInstance()->setBool("DrawFramerate", framerate->getState()); });
			// show help
			auto show_help = std::make_shared<SwitchComponent>(mWindow);
			show_help->setState(Settings::getInstance()->getBool("ShowHelpPrompts"));
			s->addWithLabel("ON-SCREEN HELP", show_help);
			s->addSaveFunc([show_help] { Settings::getInstance()->setBool("ShowHelpPrompts", show_help->getState()); });
			// quick system select (left/right in game list view)
			auto quick_sys_select = std::make_shared<SwitchComponent>(mWindow);
			quick_sys_select->setState(Settings::getInstance()->getBool("QuickSystemSelect"));
			s->addWithLabel("QUICK SYSTEM SELECT", quick_sys_select);
			s->addSaveFunc([quick_sys_select] { Settings::getInstance()->setBool("QuickSystemSelect", quick_sys_select->getState()); });
			// carousel transition option
			auto move_carousel = std::make_shared<SwitchComponent>(mWindow);
			move_carousel->setState(Settings::getInstance()->getBool("MoveCarousel"));
			s->addWithLabel("CAROUSEL TRANSITIONS", move_carousel);
			s->addSaveFunc([move_carousel] { Settings::getInstance()->setBool("MoveCarousel", move_carousel->getState()); });
			// transition style
			auto transition_style = std::make_shared< OptionListComponent<std::string> >(mWindow, "TRANSITION STYLE", false);
			std::vector<std::string> transitions;
			transitions.push_back("fade");
			transitions.push_back("slide");
			transitions.push_back("instant");
			for(auto it = transitions.cbegin(); it != transitions.cend(); it++)
				transition_style->add(*it, *it, Settings::getInstance()->getString("TransitionStyle") == *it);
			s->addWithLabel("TRANSITION STYLE", transition_style);
			s->addSaveFunc([transition_style] { Settings::getInstance()->setString("TransitionStyle", transition_style->getSelected()); });
			// theme set
			auto themeSets = ThemeData::getThemeSets();
			if(!themeSets.empty())
			{
				auto selectedSet = themeSets.find(Settings::getInstance()->getString("ThemeSet"));
				if(selectedSet == themeSets.end())
					selectedSet = themeSets.begin();
				auto theme_set = std::make_shared< OptionListComponent<std::string> >(mWindow, "THEME SET", false);
				for(auto it = themeSets.begin(); it != themeSets.end(); it++)
					theme_set->add(it->first, it->first, it == selectedSet);
				s->addWithLabel("THEME SET", theme_set);
				Window* window = mWindow;
				s->addSaveFunc([window, theme_set]
				{
					bool needReload = false;
					if(Settings::getInstance()->getString("ThemeSet") != theme_set->getSelected())
						needReload = true;
					Settings::getInstance()->setString("ThemeSet", theme_set->getSelected());
					if(needReload)
						ViewController::get()->reloadAll(); // TODO - replace this with some sort of signal-based implementation
				});
			}
			mWindow->pushGui(s);
		});

		addEntry("SOUND SETTINGS", 0x777777FF, true, [this] {
			auto s = new GuiSettings(mWindow, "SOUND SETTINGS");
			// volume
			auto volume = std::make_shared<SliderComponent>(mWindow, 0.f, 100.f, 1.f, "%");
			volume->setValue((float)VolumeControl::getInstance()->getVolume());
			s->addWithLabel("SYSTEM VOLUME", volume);
			s->addSaveFunc([volume] { VolumeControl::getInstance()->setVolume((int)round(volume->getValue())); });
			#ifdef _RPI_
				// volume control device
				auto vol_dev = std::make_shared< OptionListComponent<std::string> >(mWindow, "AUDIO DEVICE", false);
				std::vector<std::string> transitions;
				transitions.push_back("PCM");
				transitions.push_back("Speaker");
				transitions.push_back("Master");
				for(auto it = transitions.cbegin(); it != transitions.cend(); it++)
					vol_dev->add(*it, *it, Settings::getInstance()->getString("AudioDevice") == *it);
				s->addWithLabel("AUDIO DEVICE", vol_dev);
				s->addSaveFunc([vol_dev] {
					Settings::getInstance()->setString("AudioDevice", vol_dev->getSelected());
					VolumeControl::getInstance()->deinit();
					VolumeControl::getInstance()->init();
				});
			#endif
			// disable sounds
			auto sounds_enabled = std::make_shared<SwitchComponent>(mWindow);
			sounds_enabled->setState(Settings::getInstance()->getBool("EnableSounds"));
			s->addWithLabel("ENABLE SOUNDS", sounds_enabled);
			s->addSaveFunc([sounds_enabled] { Settings::getInstance()->setBool("EnableSounds", sounds_enabled->getState()); });
			mWindow->pushGui(s);
		});

		addEntry("CONFIGURE INPUT", 0x777777FF, true, [this] {
			Window* window = mWindow;
			window->pushGui(new GuiMsgBox(window, "ARE YOU SURE YOU WANT TO CONFIGURE INPUT?", "YES",
				[window] {
					window->pushGui(new GuiDetectDevice(window, false, nullptr));
				}, "NO", nullptr)
			);
		});

		addEntry("QUIT", 0x777777FF, true, [this] {
			auto s = new GuiSettings(mWindow, "QUIT");
			Window* window = mWindow;
			ComponentListRow row;
			row.makeAcceptInputHandler([window] {
				window->pushGui(new GuiMsgBox(window, "REALLY RESTART?", "YES",
				[] {
					if(quitES("/tmp/es-restart") != 0)
						LOG(LogWarning) << "Restart terminated with non-zero result!";
				}, "NO", nullptr));
			});
			row.addElement(std::make_shared<TextComponent>(window, "RESTART EMULATIONSTATION", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
			s->addRow(row);
			row.elements.clear();
			row.makeAcceptInputHandler([window] {
				window->pushGui(new GuiMsgBox(window, "REALLY RESTART?", "YES",
				[] {
					if(quitES("/tmp/es-sysrestart") != 0)
						LOG(LogWarning) << "Restart terminated with non-zero result!";
				}, "NO", nullptr));
			});
			row.addElement(std::make_shared<TextComponent>(window, "RESTART SYSTEM", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
			s->addRow(row);
			row.elements.clear();
			row.makeAcceptInputHandler([window] {
				window->pushGui(new GuiMsgBox(window, "REALLY SHUTDOWN?", "YES",
				[] {
					if(quitES("/tmp/es-shutdown") != 0)
						LOG(LogWarning) << "Shutdown terminated with non-zero result!";
				}, "NO", nullptr));
			});
			row.addElement(std::make_shared<TextComponent>(window, "SHUTDOWN SYSTEM", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
			s->addRow(row);
			row.elements.clear();
			row.makeAcceptInputHandler([window] {
				window->pushGui(new GuiMsgBox(window, "REALLY QUIT?", "YES",
				[] {
					SDL_Event ev;
					ev.type = SDL_QUIT;
					SDL_PushEvent(&ev);
				}, "NO", nullptr));
			});
			row.addElement(std::make_shared<TextComponent>(window, "QUIT EMULATIONSTATION", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
			s->addRow(row);
			mWindow->pushGui(s);
		});

		std::string  buildDate = (Settings::getInstance()->getBool("Debug") ? std::string( "   (" + strToUpper(PROGRAM_BUILT_STRING) + ")") : (""));
		mVersion.setFont(Font::get(FONT_SIZE_SMALL));
		mVersion.setColor(0x5E5E5EFF);
		mVersion.setText("EMULATIONSTATION V" + strToUpper(PROGRAM_VERSION_STRING) + buildDate);
		mVersion.setHorizontalAlignment(ALIGN_CENTER);

		addChild(&mMenu);
		addChild(&mVersion);

		setSize(mMenu.getSize());
		setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);

	}

}

void GuiMenu::onSizeChanged()
{
	mVersion.setSize(mSize.x(), 0);
	mVersion.setPosition(0, mSize.y() - mVersion.getSize().y());
}

void GuiMenu::addEntry(const char* name, unsigned int color, bool add_arrow, const std::function<void()>& func)
{
	std::shared_ptr<Font> font = Font::get(FONT_SIZE_MEDIUM);

	// populate the list
	ComponentListRow row;
	row.addElement(std::make_shared<TextComponent>(mWindow, name, font, color), true);

	if(add_arrow)
	{
		std::shared_ptr<ImageComponent> bracket = makeArrow(mWindow);
		row.addElement(bracket, false);
	}

	row.makeAcceptInputHandler(func);

	mMenu.addRow(row);
}

bool GuiMenu::input(InputConfig* config, Input input)
{
	if(GuiComponent::input(config, input))
		return true;

	if((config->isMappedTo("b", input) || config->isMappedTo("start", input)) && input.value != 0)
	{
		delete this;
		return true;
	}

	return false;
}

HelpStyle GuiMenu::getHelpStyle()
{
	HelpStyle style = HelpStyle();
	style.applyTheme(ViewController::get()->getState().getSystem()->getTheme(), "system");
	return style;
}

std::vector<HelpPrompt> GuiMenu::getHelpPrompts()
{
	std::vector<HelpPrompt> prompts;
	prompts.push_back(HelpPrompt("up/down", "choose"));
	prompts.push_back(HelpPrompt("a", "select"));
	prompts.push_back(HelpPrompt("start", "close"));
	return prompts;
}
