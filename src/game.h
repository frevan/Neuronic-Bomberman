#pragma once

#include "base/tgui/nel_game_tgui.h"

#include "views/overlayview.h"
#include "server/server.h"
#include "client/client.h"



class TGame :	public nel::TTGUIApplication
{
private:
	std::shared_ptr<TOverlayView> Overlay;
	std::shared_ptr<TServer> Server;
	std::shared_ptr<TClient> Client;

	// factory functions
	void* CreateState_Splash();
	void* CreateState_Menu();
	void* CreateState_Options();
	void* CreateState_ServerSelect();
	void* CreateState_ServerCreate();
	void* CreateState_Lobby();
	void* CreateState_LoadMatch();
	void* CreateState_Play();
	void* CreateState_RoundEnd();
	void* CreateState_MatchEnd();

protected:
	sf::RenderWindow* CreateWindow() override;
	nel::TGameID GetInitialGameStateID() override;
	void AfterInitialization() override;
	void BeforeFinalization() override;

	std::string GetDefaultFontName() override;

public:
	TGame();
	~TGame();

	nel::Interface* RetrieveInterface(nel::TGameID id) override;
};