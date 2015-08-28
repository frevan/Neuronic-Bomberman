#pragma once

#include "nel_interfaces.h"



namespace nel {



class TObjectFactory :	public IObjectFactory
{
private:
	std::map<uint64_t, TObjectFactoryDelegate> delegates;
public:
	TObjectFactory();

	// from IObjectFactory
	void registerObjectType(uint64_t objectType, const TObjectFactoryDelegate& setDelegate) override;
	void unregisterObjectType(uint64_t objectType) override;
	void* createObject(uint64_t objectType) override;
};



};	// namespace nel