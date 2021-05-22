#include "ColonialTag.h"
#include "Configuration.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ColonialTag::ColonialTag(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (theConfiguration.isHpmEnabled())
	{
		if (!colonyTag.hpm.empty())
		{
			colonyTag.tag = colonyTag.hpm;
		}
		if (!colonyTag.hpmRegions.empty())
		{
			colonyTag.V2Regions = colonyTag.hpmRegions;
		}
	}
}

void mappers::ColonialTag::registerKeys()
{
	registerKeyword("tag", [this](const std::string& unused, std::istream& theStream) {
		colonyTag.tag = commonItems::singleString(theStream).getString();
	});
	registerKeyword("hpm", [this](const std::string& unused, std::istream& theStream) {
		colonyTag.hpm = commonItems::singleString(theStream).getString();
	});
	registerKeyword("eu4region", [this](const std::string& unused, std::istream& theStream) {
		colonyTag.EU4Regions.insert(commonItems::singleString(theStream).getString());
	});
	registerKeyword("v2region", [this](const std::string& unused, std::istream& theStream) {
		colonyTag.V2Regions.insert(commonItems::singleString(theStream).getString());
	});
	registerKeyword("hpm_region", [this](const std::string& unused, std::istream& theStream) {
		colonyTag.hpmRegions.insert(commonItems::singleString(theStream).getString());
	});
	registerKeyword("cultureGroup", [this](const std::string& unused, std::istream& theStream) {
		colonyTag.cultureGroups.insert(commonItems::singleString(theStream).getString());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::ColonyStruct::match(const std::string& eu4Region, const std::string& v2Region, const std::string& cultureGroup) const
{
	if (!eu4Region.empty() && !EU4Regions.empty() && !EU4Regions.contains(eu4Region))
		return false;
	if (eu4Region.empty() && !EU4Regions.empty())
		return false;
	if (!v2Region.empty() && !V2Regions.empty() && !V2Regions.contains(v2Region))
		return false;
	if (v2Region.empty() && !V2Regions.empty())
		return false;
	if (!cultureGroup.empty() && !cultureGroups.empty() && !cultureGroups.contains(cultureGroup))
		return false;
	if (cultureGroup.empty() && !cultureGroups.empty())
		return false;
	return true;
}
