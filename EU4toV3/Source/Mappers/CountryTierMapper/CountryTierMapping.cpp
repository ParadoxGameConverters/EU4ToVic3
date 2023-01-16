#include "CountryTierMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::CountryTierMapping::CountryTierMapping(std::istream& theStream, std::string theName): name(std::move(theName))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CountryTierMapping::registerKeys()
{
	registerKeyword("size", [this](std::istream& theStream) {
		size = commonItems::getInt(theStream);
	});
	registerKeyword("rank", [this](std::istream& theStream) {
		rank = commonItems::getInt(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::CountryTierMapping::matchTier(const int incRank, const size_t incSize) const
{
	if (size && incSize > *size)
		return std::nullopt;
	if (incRank > rank)
		return std::nullopt;
	return name;
}
