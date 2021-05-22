#include "ProvinceMappingsVersion.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ProvinceMappingsVersion::ProvinceMappingsVersion(const std::string& versionString, std::istream& theStream): version(GameVersion(versionString))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ProvinceMappingsVersion::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const ProvinceMapping newMapping(theStream);
		if (newMapping.getEU4Provinces().empty() && newMapping.getVic2Provinces().empty())
			return; // this is a comment, ignore.
		mappings.push_back(newMapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
