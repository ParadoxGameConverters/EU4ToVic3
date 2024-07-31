#include "Good.h"

V3::Good::Good(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::Good::registerKeys()
{
	registerKeyword("cost", [this](std::istream& theStream) {
		cost = commonItems::getInt(theStream);
	});
	registerKeyword("local", [this](std::istream& theStream) {
		local = "yes" == commonItems::getString(theStream);
	});
	IgnoreUnregisteredItems();
}
