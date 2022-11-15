#include "ProvinceMappingsVersion.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::ProvinceMappingsVersion::ProvinceMappingsVersion(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ProvinceMappingsVersion::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const auto newMapping = ProvinceMapping(theStream);
		if (newMapping.getV3Provinces().empty() && newMapping.getEU4Provinces().empty()) // This is a comment or empty entry.
			return;
		mappings.push_back(newMapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
