#include "PartyType.h"
#include "CommonRegexes.h"
#include "Configuration.h"
#include "ParserHelpers.h"

mappers::PartyType::PartyType(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	if (!theConfiguration.isHpmEnabled())
	{
		policies.erase("social_policy");
	}

}

void mappers::PartyType::registerKeys()
{
	registerKeyword("name", [this](const std::string& unused, std::istream& theStream) {
		name = commonItems::singleString(theStream).getString();
	});
	registerKeyword("ideology", [this](const std::string& unused, std::istream& theStream) {
		ideology = commonItems::singleString(theStream).getString();
	});
	registerKeyword("start_date", [this](const std::string& unused, std::istream& theStream) {
		startDate = date(commonItems::singleString(theStream).getString());
	});
	registerKeyword("end_date", [this](const std::string& unused, std::istream& theStream) {
		endDate = date(commonItems::singleString(theStream).getString());
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& policyName, std::istream& theStream) {
		policies.insert(std::make_pair(policyName, commonItems::singleString(theStream).getString()));
	});
}
