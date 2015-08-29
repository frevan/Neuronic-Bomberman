#include "nel_factory.h"

#include <cassert>



namespace nel {

// --- TObjectFactory ---

TObjectFactory::TObjectFactory()
:	IObjectFactory(),
	Delegates()
{
}

void TObjectFactory::RegisterObjectType(TObjectType objectType, IObjectFactoryDelegate setDelegate)
{
	Delegates[objectType] = setDelegate;
	assert(Delegates[objectType] != nullptr);
}

void TObjectFactory::UnregisterObjectType(TObjectType objectType)
{
	Delegates.erase(objectType);
}

void* TObjectFactory::CreateObject(TObjectType objectType)
{
	assert(Delegates[objectType] != nullptr);

	auto delegate = Delegates[objectType];
	return delegate();
}

};	// namespace nel