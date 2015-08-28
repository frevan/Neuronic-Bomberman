#include "nel_factory.h"



namespace nel {



// --- TObjectFactory ---

TObjectFactory::TObjectFactory()
:	IObjectFactory(),
	Delegates()
{
}

void TObjectFactory::RegisterObjectType(uint64_t objectType, const TObjectFactoryDelegate& setDelegate)
{
	Delegates[objectType] = setDelegate;
}

void TObjectFactory::UnregisterObjectType(uint64_t objectType)
{
	Delegates.erase(objectType);
}

void* TObjectFactory::CreateObject(uint64_t objectType)
{
	return Delegates[objectType]();
}



};	// namespace nel