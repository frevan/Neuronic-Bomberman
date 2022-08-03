#include "view.h"

// --- TView --- 

TView::TView(TGame* SetGame, TViewType SetType)
:	Game(SetGame),
	Type(SetType),
	Visible(true),
	ID(0)
{
}

void TView::OnAttach()
{
}

void TView::OnDetach()
{
}

bool TView::IsVisible()
{
	return Visible;
}

void TView::SetVisible(bool NewValue)
{
	Visible = NewValue;
}

void TView::Draw(sf::RenderTarget* Target)
{
}

bool TView::ProcessInput(TInputID InputID, float Value)
{
	return false;
}

void TView::StateChanged()
{
}

// --- TTGUISystemView ---

TTGUISystemView::TTGUISystemView(TGame* SetGame, tgui::Gui* SetGUI)
:	TView(SetGame, TViewType::VT_SYSTEMVIEW),
	GUI(SetGUI)
{
}

void TTGUISystemView::Draw(sf::RenderTarget* target)
{
	GUI->draw();
};

// --- TTGUIView ---

TTGUIView::TTGUIView(TGame* SetGame, TViewType SetType, tgui::Gui* SetGUI)
: TView(SetGame, SetType),
	GUI(SetGUI)
{
}

TTGUIView::~TTGUIView()
{
	RemoveAllWidgetsFromGUI();
}

void TTGUIView::OnAttach()
{
	TView::OnAttach();

	RemoveAllWidgetsFromGUI();
	CreateWidgets();
}

void TTGUIView::OnDetach()
{
	RemoveAllWidgetsFromGUI();

	TView::OnDetach();
}

void TTGUIView::AddWidgetToGUI(tgui::Widget::Ptr widget, const std::string& widgetName)
{
	Widgets.push_back(widget);
	GUI->add(widget, widgetName);
}

void TTGUIView::RemoveWidgetFromGUI(tgui::Widget::Ptr widget)
{
	GUI->remove(widget);
	for (auto it = Widgets.begin(); it != Widgets.end(); it++)
	{
		if ((*it) == widget)
		{
			Widgets.erase(it);
			break;
		}
	}
}

void TTGUIView::RemoveAllWidgetsFromGUI()
{
	for (auto it = Widgets.begin(); it != Widgets.end(); it++)
		GUI->remove(*it);
	Widgets.clear();
}

void TTGUIView::Draw(sf::RenderTarget* Target)
{
}