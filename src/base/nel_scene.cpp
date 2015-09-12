#include "nel_scene.h"



namespace nel {

TScene::TScene(TViewType setType)
:	IScene(),
	ViewType(setType),
	Visible(true)
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

bool TScene::IsVisible()
{
	return Visible;
}

void TScene::SetVisible(bool newValue)
{
	Visible = newValue;
}

};	// namespace nel