#pragma once

#include "nel_interfaces.h"



namespace nel {

class TScene	: public IScene
{
protected:
	TViewType ViewType;
	IApplication* Application;
public:
	TScene(TViewType setType = VT_HUMANVIEW);
	~TScene() override;

	// from IScene
	void OnAttach(IApplication* setApplication) override;
	void OnDetach() override;
	void Draw(sf::RenderTarget* target) override;
	TViewType GetType() override;
};

};