#include "nel_view.h"



namespace nel {

TView::TView(IStateMachine* setStateMachine, TViewType setType)
:	IView(),
	StateMachine(setStateMachine),
	ViewType(setType),
	Visible(true)
{
}

TView::~TView()
{
}

void TView::OnAttach(IApplication* setApplication)
{
	Application = setApplication;
}

void TView::OnDetach()
{
}

void TView::Draw(sf::RenderTarget* target)
{
}

IView::TViewType TView::GetType()
{
	return ViewType;
}

bool TView::IsVisible()
{
	return Visible;
}

void TView::SetVisible(bool newValue)
{
	Visible = newValue;
}

};	// namespace nel