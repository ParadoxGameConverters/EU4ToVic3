#include "ColonialFlagRegion.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ColonialFlagRegion::ColonialFlagRegion(std::istream& theStream, const std::string& region)
{
	registerKeys(region);
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ColonialFlagRegion::registerKeys(const std::string& region)
{
	registerKeyword("flag", [this, region](const std::string& unused, std::istream& theStream) {
		ColonialFlag newFlag(theStream, region);
		regionalFlags.insert(std::make_pair(newFlag.getName(), newFlag));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
