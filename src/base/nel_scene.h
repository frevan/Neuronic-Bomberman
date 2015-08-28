#pragma once

#include "nel_interfaces.h"



namespace nel {

class TScene	: public IScene
{
protected:
	TViewType fType;
	IApplication* Application;
public:
	TScene(TViewType setType = VT_HUMANVIEW);
	~TScene() override;

	// from IScene
	void onAttach(IApplication* setApplication) override;
	void onDetach() override;
	TViewType getType() override;
};

};