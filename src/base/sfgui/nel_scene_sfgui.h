#pragma once

#include "../nel_scene.h"

#include <SFML/Graphics.hpp>
#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>



namespace nel {

class TSFGUIScene :	public TScene,
					public nel::IEventHandler
{
private:
	sf::Clock GUIClock;

public:
	std::unique_ptr<sfg::Desktop> Desktop; 

	TSFGUIScene(TViewType setType = VT_HUMANVIEW);
	~TSFGUIScene() override;

	// from IScene
	void onAttach(IApplication* setApplication) override;
	void onDetach() override;
	void draw(sf::RenderTarget* target) override;

	// from IEventHandler
	bool processEvent(const sf::Event& event) override;
};

};	// namespace nel