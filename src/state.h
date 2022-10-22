#pragma once

class TState;

#include "game.h"
#include "view.h"

const int GAMESTATE_NONE = 0;
const int GAMESTATE_MENU = 1;
const int GAMESTATE_CONNECTING = 2;
const int GAMESTATE_LOBBY = 3;
const int GAMESTATE_MATCH = 4;
const int GAMESTATE_ENDOFROUND = 5;
const int GAMESTATE_ENDOFMATCH = 6;
const int GAMESTATE_CONNECTTOSERVER = 7;
const int GAMESTATE_QUIT = 1000;

class TState
{
protected:
	TGame* Game;
public:
	int ID;
	TView* View;

	TState(TGame* SetGame, int SetID) : Game(SetGame), ID(SetID), View(nullptr) {};

	virtual void Init() = 0;
	virtual void Finish();
	virtual void GameStateChanged();
};

class TMenuState : public TState
{
public:
	TMenuState(TGame* SetGame) : TState(SetGame, GAMESTATE_MENU) {};
	virtual void Init() override;
	virtual void Finish() override;
};

class TConnectingState : public TState
{
public:
	TConnectingState(TGame* SetGame) : TState(SetGame, GAMESTATE_CONNECTING) {};
	virtual void Init() override;
	virtual void Finish() override;
};

class TLobbyState : public TState
{
public:
	TLobbyState(TGame* SetGame) : TState(SetGame, GAMESTATE_LOBBY) {};
	virtual void Init() override;
	virtual void Finish() override;
};

class TMatchState : public TState
{
public:
	TMatchState(TGame* SetGame) : TState(SetGame, GAMESTATE_MATCH) {};
	virtual void Init() override;
	virtual void Finish() override;
};

class TEndOfRoundState : public TState
{
public:
	TEndOfRoundState(TGame* SetGame) : TState(SetGame, GAMESTATE_ENDOFROUND) {};
	virtual void Init() override;
	virtual void Finish() override;
};

class TEndOfMatchState : public TState
{
public:
	TEndOfMatchState(TGame* SetGame) : TState(SetGame, GAMESTATE_ENDOFMATCH) {};
	virtual void Init() override;
	virtual void Finish() override;
};

class TConnectToServerState : public TState
{
public:
	TConnectToServerState(TGame* SetGame) : TState(SetGame, GAMESTATE_CONNECTTOSERVER) {};
	virtual void Init() override;
	virtual void Finish() override;
};
