#pragma once

#include <cstdint>

typedef uint32_t TViewID;

class TView;
class TTGUISystemView;

#include <TGUI/TGUI.hpp>

#include "game.h"
#include "inputmap.h"
#include "game.h"
#include "gamedata.h"

enum class TViewType
{
	VT_OVERLAY = -1, // drawn on top of everything
	VT_SYSTEMVIEW = 0, // any other view
	VT_HUMANVIEW // local human player
};

class TView
{
private:
public:
	bool Visible;
	TGame* Game;
	TViewType Type;
	TViewID ID;

	TView(TGame* SetGame, TViewType SetType);

	virtual void OnAttach();
	virtual void OnDetach();
	virtual bool IsVisible();
	virtual void SetVisible(bool NewValue);
	virtual void Draw(sf::RenderTarget* target);
	virtual bool ProcessInput(TInputID InputID, float Value);
	virtual void StateChanged();
	virtual void Process(TGameTime Delta);
};

class TTGUISystemView : public TView
{
private:
	tgui::Gui* GUI;
public:
	TTGUISystemView(TGame* SetGame, tgui::Gui* SetGUI);
	
	// from TViewInterface
	void Draw(sf::RenderTarget* target) override;
};

class TTGUIView : public TView
{
private:
	std::vector<tgui::Widget::Ptr> Widgets;
protected:
	tgui::Gui* GUI;

	void AddWidgetToGUI(tgui::Widget::Ptr widget, const std::string& widgetName = "");
	void RemoveWidgetFromGUI(tgui::Widget::Ptr widget);
	void RemoveAllWidgetsFromGUI();

	virtual void CreateWidgets() {};
public:
	TTGUIView(TGame* SetGame, TViewType SetType, tgui::Gui* SetGUI);
	~TTGUIView();

	// from TView
	void OnAttach() override;
	void OnDetach() override;
	void Draw(sf::RenderTarget* Target) override;
};