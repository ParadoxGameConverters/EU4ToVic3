#include "BuildingGroup.h"
#include "CommonRegexes.h"
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
	registerKeyword("capped_by_resources", [this](std::istream& theStream) {
		resourceCapped = (commonItems::getString(theStream) == "yes");
	});
	registerKeyword("land_usage", [this](std::istream& theStream) {
		arableCapped = (commonItems::getString(theStream) == "rural");
		if (arableCapped)
		{
			resourceCapped = true;
		}
	});
	registerKeyword("parent_group", [this](std::istream& theStream) {
		parent = commonItems::getString(theStream);
	});
	registerKeyword("category", [this](std::istream& theStream) {
		category = commonItems::getString(theStream);
	});
	registerKeyword("infrastructure_usage_per_level", [this](std::istream& theStream) {
		infrastructureCost = commonItems::getDouble(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
