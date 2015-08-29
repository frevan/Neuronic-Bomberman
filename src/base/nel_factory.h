#pragma once

#include "nel_interfaces.h"



namespace nel {



class TObjectFactory :	public IObjectFactory
{
private:
	std::map<TObjectType, IObjectFactoryDelegate> Delegates;
public:
	TObjectFactory();

	// from IObjectFactory
	void RegisterObjectType(TObjectType objectType, IObjectFactoryDelegate setDelegate) override;
	void UnregisterObjectType(TObjectType objectType) override;
	void* CreateObject(TObjectType objectType) override;
};



};	// namespace nel