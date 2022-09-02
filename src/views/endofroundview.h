#pragma once

#include "../view.h"

class TEndOfRoundView : public TView
{
private:
public:
	TEndOfRoundView(TGame* SetGame);
	~TEndOfRoundView();

	// from TView
	void Draw(sf::RenderTarget* target) override;
	bool ProcessInput(TInputID InputID, float Value) override;
};