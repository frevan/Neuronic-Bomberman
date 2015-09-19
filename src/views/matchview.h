#pragma once

#include "../base/nel_interfaces.h"
#include "../base/nel_view.h"

#include <SFML/Graphics.hpp>



class TMatchView :	public nel::TView
{
private:
public:
	TMatchView(nel::IStateMachine* setStateMachine);
	~TMatchView() override;

	// from IScene
	void OnAttach(nel::IApplication* setApplication) override;
	void OnDetach() override;
	void Draw(sf::RenderTarget* target) override;
};