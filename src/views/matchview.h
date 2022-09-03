#pragma once

#include "../view.h"

class TMatchView : public TView
{
private:
	void DeterminePlayerActionFromInput(TInputID& InputID, float& Value, int& PlayerIndex);
public:
	TMatchView(TGame* SetGame);
	~TMatchView();

	// from TView
	void Draw(sf::RenderTarget* target) override;
	bool ProcessInput(TInputID InputID, float Value) override;
	void Process(TGameTime Delta) override;
};