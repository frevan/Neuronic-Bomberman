#include "basegame.h"

#include <algorithm>
#include <cstdint>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>



// --- TGameApplication ---

TGameApplication::TGameApplication()
:	appPath()
{
}

TGameApplication::~TGameApplication()
{
}

bool TGameApplication::initialize(const std::string& filename)
{
	// set application path
	appPath = filename;
	#ifdef WIN32
	std::replace(appPath.begin(), appPath.end(), '\\', '/');
	#endif
	size_t found = appPath.find_last_of('/');
	appPath = appPath.substr(0, found+1);

	// create window
	sf::Window* w = createWindow();
	if (!w)
		return false;
	window.reset(w);	// set our smart pointer to the newly created window

	// done
	return true;
}

void TGameApplication::finalize()
{
	window.reset();	// destroy window
}

void TGameApplication::execute()
{
	sf::Clock clock;

	const int TICKS_PER_SECOND = 25;
    const int SKIP_TICKS = 1000 / TICKS_PER_SECOND;
    const int MAX_FRAMESKIP = 5;

	uint32_t startTickForFPS = clock.getElapsedTime().asMilliseconds();
	uint32_t nextGameTick = startTickForFPS;
    int loops = 0;
    //float interpolation;

	uint32_t prevTickTime = clock.getElapsedTime().asMilliseconds();
	//uint32_t nowTime;

	while (window->isOpen())
	{
        // handle events
        sf::Event event;
        while (window->pollEvent(event))
        {
            // Window closed or escape key pressed: exit
            if ((event.type == sf::Event::Closed) || 
               ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)))
            {
                window->close();
                break;
            }
		}

		/*
		// check for messages
		while (processOneEvent())
		{
		}

		if (shouldQuit())	if (processQuitRequest())
			break;

		// process events
		TBaseEventManager::get()->process(20);	// process events for up to 20 milliseconds

		if (shouldQuit())	if (processQuitRequest())
			break;
		*/

		/*
		// let the scene process a tick
        loops = 0;
		nowTime = clock.getElapsedTime().asMilliseconds();
        while (nowTime > nextGameTick && loops < MAX_FRAMESKIP) 
		{			
			for (auto it = logicProcessors.begin(); it != logicProcessors.end(); it++)
				(*it)->update(nowTime-prevTickTime);
            nextGameTick += SKIP_TICKS;
            loops++;
			// get new time
			prevTickTime = nowTime;
			nowTime = app->getCurrentTime();
        }
		*/

		int curTicks = clock.getElapsedTime().asMilliseconds();

		// calculate fps
		//fpsCalculator->newFrame();

		// calculate interpolation
        //interpolation = float(curTicks + SKIP_TICKS - nextGameTick) / float(SKIP_TICKS);

		// paint		
       // window->clear(sf::Color(50, 200, 50));
        window->display();
	}
}
