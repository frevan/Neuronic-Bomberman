#include "nel_scene.h"



namespace nel {

TScene::TScene(TViewType setType)
:	IScene(),
	ViewType(setType)
{
}

TScene::~TScene()
{
}

void TScene::OnAttach(IApplication* setApplication)
{
	Application = setApplication;
}

void TScene::OnDetach()
{
}

void TScene::Draw(sf::RenderTarget* target)
{
}

IScene::TViewType TScene::GetType()
{
	return ViewType;
}

};	// namespace nel