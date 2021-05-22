#include "CountryFlags.h"
#include "CountryFlagsMapping.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::CountryFlags::CountryFlags()
{
	registerKeys();
	parseFile("configurables/flag_mappings.txt");
	clearRegisteredKeywords();
}

mappers::CountryFlags::CountryFlags(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CountryFlags::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const CountryFlagsMapping newMapping(theStream);
		if (!newMapping.getEU4Flag().empty())
			for (const auto& v2Flag: newMapping.getV2Flags())
				flags.insert(std::make_pair(newMapping.getEU4Flag(), v2Flag));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}