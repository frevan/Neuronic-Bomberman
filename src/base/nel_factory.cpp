#include "nel_factory.h"



namespace nel {



// --- TObjectFactory ---

TObjectFactory::TObjectFactory()
:	IObjectFactory(),
	delegates()
{
}

void TObjectFactory::registerObjectType(uint64_t objectType, const TObjectFactoryDelegate& setDelegate)
{
	delegates[objectType] = setDelegate;
}

void TObjectFactory::unregisterObjectType(uint64_t objectType)
{
	delegates.erase(objectType);
}

void* TObjectFactory::createObject(uint64_t objectType)
{
	return delegates[objectType]();
}



};	// namespace nel