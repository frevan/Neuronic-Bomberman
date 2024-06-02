#include "overlayview.h"

#include <TGUI/TGUI.hpp>

#include "../actions.h"

TOverlayView::TOverlayView(TGame* SetGame, tgui::Gui* SetGUI)
:	TTGUIView(SetGame, TViewType::VT_OVERLAY, SetGUI),
	BackgroundShape()
{
}

TOverlayView::~TOverlayView()
{
}

void TOverlayView::CreateWidgets()
{
	BackgroundShape.setFillColor(sf::Color(16, 16, 16, 200));
	BackgroundShape.setPosition(sf::Vector2f(0, 0));
	BackgroundShape.setSize(sf::Vector2f(800, 600));
}

bool TOverlayView::ProcessInput(TInputID InputID, float Value)
{
	// only handle key presses
	if (!IsInputPressed(Value))
		return false;

	bool handled = true;
	switch (InputID)
	{
		case actionToggleOverlay:
			Game->ToggleOverlay();
			break;

		default:
			handled = false;
			break;
	};

	return handled;
}

void TOverlayView::StateChanged()
{
}

void TOverlayView::Draw(sf::RenderTarget* target)
{
	TTGUIView::Draw(target);

	if (Game->OverlayIsVisible)
		target->draw(BackgroundShape);
}