#include "nel_scene_sfgui.h"

#include <SFGUI/SFGUI.hpp>
#include <SFGUI/Widgets.hpp>



namespace nel {

TSFGUIScene::TSFGUIScene(TViewType setType)
:	TScene(setType),
	Desktop(),
	GUIClock()
{
}

TSFGUIScene::~TSFGUIScene()
{
}

void TSFGUIScene::onAttach(IApplication* setApplication)
{
	TScene::onAttach(setApplication);

	Desktop = std::make_unique<sfg::Desktop>();
	GUIClock.restart();
}

void TSFGUIScene::onDetach()
{
	Desktop.reset(nullptr);

	TScene::onDetach();
}

void TSFGUIScene::draw(sf::RenderTarget* target)
{
	float seconds = GUIClock.restart().asSeconds();
	Desktop->Update(seconds);
}

bool TSFGUIScene::processEvent(const sf::Event& event)
{
	Desktop->HandleEvent(event);
	return true;
}

};	// namespace nel