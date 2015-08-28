#pragma once

#include "nel_interfaces.h"



namespace nel {



class TObjectFactory :	public IObjectFactory
{
private:
	std::map<uint64_t, TObjectFactoryDelegate> Delegates;
public:
	TObjectFactory();

	// from IObjectFactory
	void RegisterObjectType(uint64_t objectType, const TObjectFactoryDelegate& setDelegate) override;
	void UnregisterObjectType(uint64_t objectType) override;
	void* CreateObject(uint64_t objectType) override;
};



};	// namespace nel