#pragma once

#include <string>
#include <memory>

#include <SFML/Window.hpp>



class TGameApplication
{
private:
	std::unique_ptr<sf::Window> window;
protected:
	virtual sf::Window* createWindow() = 0;

public:
	std::string appPath;

	TGameApplication();
	~TGameApplication();

	bool initialize(const std::string& filename);
	virtual void finalize();
	virtual void execute();	
};
