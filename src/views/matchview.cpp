#include "matchview.h"



TMatchView::TMatchView(nel::IStateMachine* setStateMachine)
:	TView(setStateMachine, TViewType::VT_HUMANVIEW)
{
}

TMatchView::~TMatchView()
{
}

void TMatchView::OnAttach(nel::IApplication* setApplication)
{
	TView::OnAttach(setApplication);
}

void TMatchView::OnDetach()
{
	TView::OnDetach();
}

void TMatchView::Draw(sf::RenderTarget* target)
{
}
