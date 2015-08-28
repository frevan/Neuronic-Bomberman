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

void TScene::onAttach(IApplication* setApplication)
{
	Application = setApplication;
}

void TScene::onDetach()
{
}

IScene::TViewType TScene::getType()
{
	return fType;
}

};	// namespace nel