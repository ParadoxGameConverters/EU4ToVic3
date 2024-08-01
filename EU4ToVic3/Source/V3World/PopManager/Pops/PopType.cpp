#include "PopType.h"

V3::PopType::PopType(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::PopType::registerKeys()
{
	registerKeyword("working_adult_ratio", [this](std::istream& theStream) {
		dependentRatio = commonItems::getDouble(theStream);
	});
	registerKeyword("consumption_mult", [this](std::istream& theStream) {
		consumptionRate = commonItems::getDouble(theStream);
	});
	IgnoreUnregisteredItems();
}
