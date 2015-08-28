#include "nel_scene.h"



namespace nel {

TScene::TScene(TViewType setType)
:	IScene(),
	fType(setType)
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
	return fType;
}

};	// namespace nel