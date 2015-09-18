#pragma once

#include "nel_interfaces.h"



namespace nel {

class TView	: public IView
{
protected:
	TViewType ViewType;
	IApplication* Application;
	IStateMachine* StateMachine;
	bool Visible;
public:
	TView(IStateMachine* setStateMachine, TViewType setType = VT_HUMANVIEW);
	~TView() override;

	// from IScene
	void OnAttach(IApplication* setApplication) override;
	void OnDetach() override;
	void Draw(sf::RenderTarget* target) override;
	TViewType GetType() override;
	bool IsVisible() override;
	void SetVisible(bool newValue) override;
};

};