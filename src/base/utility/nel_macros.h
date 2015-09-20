#include "../nel_interfaces.h"

#define DEFINE_GAMEID(name, namestring) \
	const nel::TGameID name = std::hash<std::string>()(std::string(namestring));

#define DEFINE_OBJECTTYPE(name, namestring) \
	const nel::TObjectType name = std::hash<std::string>()(std::string(namestring));