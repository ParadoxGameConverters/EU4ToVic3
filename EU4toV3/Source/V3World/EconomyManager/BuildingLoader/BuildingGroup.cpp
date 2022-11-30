#include "BuildingGroup.h"
#include "ParserHelpers.h"

void V3::BuildingGroup::loadBuildingGroup(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::BuildingGroup::registerKeys()
{
	registerKeyword("parent_group", [this](std::istream& theStream) {
		parent = commonItems::getString(theStream);
	});
	registerKeyword("infrastructure_usage_per_level", [this](std::istream& theStream) {
		infrastructureCost = commonItems::getInt(theStream);
	});
}
