#include "LeaderTraitTypes.h"
#include "CommonRegexes.h"

mappers::LeaderTraitTypes::LeaderTraitTypes(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::LeaderTraitTypes::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& traitName, std::istream& theStream) {
		const LeaderTrait traitBlock(theStream, traitName);
		traits.emplace_back(traitBlock);
	});
}
