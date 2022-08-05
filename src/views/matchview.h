#pragma once

#include "../view.h"

class TMatchView : public TView
{
private:	
public:
	TMatchView(TGame* SetGame);
	~TMatchView();

	// from TView
	void Draw(sf::RenderTarget* target) override;
	bool ProcessInput(TInputID InputID, float Value) override;
};