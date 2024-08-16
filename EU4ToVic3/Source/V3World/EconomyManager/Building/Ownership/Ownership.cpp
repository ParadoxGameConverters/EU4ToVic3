#include "Ownership.h"

V3::Ownership::Ownership(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::Ownership::registerKeys()
{
	registerKeyword("weight", [this](std::istream& theStream) {
		ownership.weight = commonItems::getDouble(theStream);
	});
	registerKeyword("colonial", [this](std::istream& theStream) {
		ownership.colonialFrac = commonItems::getDouble(theStream);
	});
	registerKeyword("financial", [this](std::istream& theStream) {
		ownership.financialCenterFrac = commonItems::getDouble(theStream);
	});
	registerKeyword("recognized", [this](std::istream& theStream) {
		ownership.recognized = commonItems::getString(theStream) == "yes";
	});
	IgnoreUnregisteredItems();
}