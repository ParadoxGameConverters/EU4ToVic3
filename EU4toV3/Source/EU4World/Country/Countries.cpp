#include "Countries.h"
#include "ParserHelpers.h"

EU4::Countries::Countries(
	const GameVersion& theVersion,
	std::istream& theStream,
	const mappers::IdeaEffectMapper& ideaEffectMapper)
{
	registerKeyword("---", commonItems::ignoreItem);
	registerKeyword("REB", commonItems::ignoreItem);
	registerKeyword("PIR", commonItems::ignoreItem);
	registerKeyword("NAT", commonItems::ignoreItem);
	registerRegex("[A-Z0-9]{3}", [this, theVersion, ideaEffectMapper](const std::string& tag, std::istream& theStream)
		{
			auto country = std::make_shared<Country>(tag, theVersion, theStream, ideaEffectMapper);
			theCountries.insert(std::make_pair(tag, country));
		}
	);
	registerRegex("[a-zA-Z0-9_\\.:]+", commonItems::ignoreItem);

	parseStream(theStream);
	clearRegisteredKeywords();
}