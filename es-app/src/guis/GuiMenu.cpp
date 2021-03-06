#include "EmulationStation.h"
#include "guis/GuiMenu.h"
#include "Window.h"
#include "Sound.h"
#include "Log.h"
#include "Settings.h"
#include "guis/GuiMsgBox.h"
#include "guis/GuiSettings.h"
#include "guis/GuiScraperStart.h"
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

	// SCRAPER >
	// SOUND SETTINGS >
	// UI SETTINGS >
	// SOUND SETTINGS >
	// CONFIGURE INPUT >
	// QUIT >

	// [version]

	if(!Settings::getInstance()->getBool("ForceKiosk"))
	{
		if(!Settings::getInstance()->getBool("ForceHandheld"))
			addEntry("SCRAPER", 0x777777FF, true, [this] { openScraperSettings(); });
		if(!Settings::getInstance()->getBool("ForceHandheld"))
			addEntry("UI SETTINGS", 0x777777FF, true, [this] { openUISettings(); });
		addEntry("SOUND SETTINGS", 0x777777FF, true, [this] { openSoundSettings(); });
		if(!Settings::getInstance()->getBool("ForceHandheld"))
			addEntry("CONFIGURE INPUT", 0x777777FF, true, [this] { openConfigInput(); });
		if(!Settings::getInstance()->getBool("ForceHandheld"))
		{
			addEntry("QUIT", 0x777777FF, true, [this] { openQuitMenu(); });
		} else {
			addEntry("QUIT", 0x777777FF, false, [this] { openConfirmShutdown(); });
		}
		addChild(&mMenu);
		addVersionInfo();
		addChild(&mVersion);
		setSize(mMenu.getSize());
		setPosition((Renderer::getScreenWidth() - mSize.x()) / 2, Renderer::getScreenHeight() * 0.15f);
	}
}

void GuiMenu::openScraperSettings()
{
	auto s = new GuiSettings(mWindow, "SCRAPER");
	// scrape from
	auto scraper_list = std::make_shared< OptionListComponent< std::string > >(mWindow, "SCRAPE FROM", false);
	std::vector<std::string> scrapers = getScraperList();

	// Select either the first entry of the one read from the settings, just in case the scraper from settings has vanished.
	for(auto it = scrapers.cbegin(); it != scrapers.cend(); it++)
		scraper_list->add(*it, *it, *it == Settings::getInstance()->getString("Scraper"));

	s->addWithLabel("SCRAPE FROM", scraper_list);
	s->addSaveFunc([scraper_list] { Settings::getInstance()->setString("Scraper", scraper_list->getSelected()); });
	// scrape ratings
	auto scrape_ratings = std::make_shared<SwitchComponent>(mWindow);
	scrape_ratings->setState(Settings::getInstance()->getBool("ScrapeRatings"));
	s->addWithLabel("SCRAPE RATINGS", scrape_ratings);
	s->addSaveFunc([scrape_ratings] { Settings::getInstance()->setBool("ScrapeRatings", scrape_ratings->getState()); });
	// save scrape image to gamelist or ES folder
	auto save_to_gamelist = std::make_shared<SwitchComponent>(mWindow);
	save_to_gamelist->setState(Settings::getInstance()->getBool("ScraperSaveImageToGamelist"));
	s->addWithLabel("SAVE IMAGES LOCALLY", save_to_gamelist);
	s->addSaveFunc([save_to_gamelist] { Settings::getInstance()->setBool("ScraperSaveImageToGamelist", save_to_gamelist->getState()); });
	// scrape now
	ComponentListRow row;
	auto openScrapeNow = [this] { mWindow->pushGui(new GuiScraperStart(mWindow)); };
	std::function<void()> openAndSave = openScrapeNow;
	openAndSave = [s, openAndSave] { s->save(); openAndSave(); };
	row.makeAcceptInputHandler(openAndSave);
	auto scrape_now = std::make_shared<TextComponent>(mWindow, "SCRAPE NOW", Font::get(FONT_SIZE_MEDIUM), 0x777777FF);
	auto bracket = makeArrow(mWindow);
	row.addElement(scrape_now, true);
	row.addElement(bracket, false);
	s->addRow(row);
	mWindow->pushGui(s);
}

void GuiMenu::openUISettings()
{
	auto s = new GuiSettings(mWindow, "UI SETTINGS");
	// screensaver
	ComponentListRow screensaver_row;
	screensaver_row.elements.clear();
	screensaver_row.addElement(std::make_shared<TextComponent>(mWindow, "SCREENSAVER SETTINGS", Font::get(FONT_SIZE_MEDIUM), 0x777777FF), true);
	screensaver_row.addElement(makeArrow(mWindow), false);
	screensaver_row.makeAcceptInputHandler(std::bind(&GuiMenu::openScreensaverOptions, this));
	s->addRow(screensaver_row);
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
	s->addWithLabel("MOVE CAROUSEL", move_carousel);
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
		s->addSaveFunc([theme_set]
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
}

void GuiMenu::openScreensaverOptions()
{
	auto s = new GuiSettings(mWindow, "SCREENSAVER");
	// screensaver time
	auto screensaver_time = std::make_shared<SliderComponent>(mWindow, 0.f, 30.f, 1.f, "m");
	screensaver_time->setValue((float)(Settings::getInstance()->getInt("ScreenSaverTime") / (1000 * 60)));
	s->addWithLabel("START AFTER", screensaver_time);
	s->addSaveFunc([screensaver_time] { Settings::getInstance()->setInt("ScreenSaverTime", (int)round(screensaver_time->getValue()) * (1000 * 60)); });
	// screensaver behavior
	auto screensaver_behavior = std::make_shared< OptionListComponent<std::string> >(mWindow, "TRANSITION STYLE", false);
	std::vector<std::string> screensavers;
	screensavers.push_back("dim");
	screensavers.push_back("black");
	auto saved_screensaver_behavior = Settings::getInstance()->getString("ScreenSaverBehavior");
	// if a non supported screensaver was saved, fallback to default
	if (!any_of(screensavers, saved_screensaver_behavior))
		saved_screensaver_behavior = screensavers.at(0);
	for(auto it = screensavers.cbegin(); it != screensavers.cend(); it++)
		screensaver_behavior->add(*it, *it, saved_screensaver_behavior == *it);
	s->addWithLabel("BEHAVIOR", screensaver_behavior);
	s->addSaveFunc([screensaver_behavior] { Settings::getInstance()->setString("ScreenSaverBehavior", screensaver_behavior->getSelected()); });
	mWindow->pushGui(s);
}

void GuiMenu::openSoundSettings()
{
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
}

void GuiMenu::openConfirmShutdown()
{
	Window* window = mWindow;
	window->pushGui(new GuiMsgBox(window, "REALLY SHUTDOWN?", "YES",
		[] {
			if(quitES("/tmp/es-shutdown") != 0)
				LOG(LogWarning) << "Shutdown terminated with non-zero result!";
		}, "NO", nullptr)
	);
}

void GuiMenu::openConfigInput()
{
	Window* window = mWindow;
	window->pushGui(new GuiMsgBox(window, "ARE YOU SURE YOU WANT TO CONFIGURE INPUT?", "YES",
		[window] {
			window->pushGui(new GuiDetectDevice(window, false, nullptr));
		}, "NO", nullptr)
	);
}

void GuiMenu::addVersionInfo()
{
	std::string  buildDate = (Settings::getInstance()->getBool("Debug") ? std::string( "   (" + strToUpper(PROGRAM_BUILT_STRING) + ")") : (""));
	mVersion.setFont(Font::get(FONT_SIZE_SMALL));
	mVersion.setColor(0x5E5E5EFF);
	mVersion.setText("EMULATIONSTATION V" + strToUpper(PROGRAM_VERSION_STRING) + buildDate);
	mVersion.setHorizontalAlignment(ALIGN_CENTER);
}

void GuiMenu::openQuitMenu()
{
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
	row.makeAcceptInputHandler([this] { openConfirmShutdown(); });
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
