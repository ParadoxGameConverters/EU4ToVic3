#include "EU4Diplomacy.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::EU4Diplomacy::EU4Diplomacy(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::EU4Diplomacy::registerKeys()
{
	registerRegex("royal_marriage|guarantee|alliance", [this](const std::string& agreementClass, std::istream& theStream) {
		EU4Agreement newAgreement(theStream);
		newAgreement.setAgreementType(agreementClass);
		agreements.push_back(newAgreement);
	});
	// new saves use dependency for subjects. We alter nothing.
	registerKeyword("dependency", [this](const std::string& unused, std::istream& theStream) {
		const EU4Agreement newAgreement(theStream);
		agreements.push_back(newAgreement);
	});
	// support for old saves, colonial is a special case we don't drop to vassal.
	registerKeyword("is_colonial", [this](const std::string& unused, std::istream& theStream) {
		EU4Agreement newAgreement(theStream);
		newAgreement.setAgreementType("colonial");
		agreements.push_back(newAgreement);
	});
	// support for old saves, all of these drop to vassal.
	registerRegex("vassal|protectorate|is_march|sphere|union", [this](const std::string& unused, std::istream& theStream) {
		EU4Agreement newAgreement(theStream);
		newAgreement.setAgreementType("vassal");
		agreements.push_back(newAgreement);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
