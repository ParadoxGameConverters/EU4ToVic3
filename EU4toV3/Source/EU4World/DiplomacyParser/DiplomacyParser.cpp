#include "DiplomacyParser.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void EU4::DiplomacyParser::loadDiplomacy(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::DiplomacyParser::registerKeys()
{
	registerRegex("royal_marriage|guarantee|alliance|steer_trade|transfer_trade_power", [this](const std::string& agreementClass, std::istream& theStream) {
		EU4Agreement newAgreement(theStream);
		newAgreement.setAgreementType(agreementClass);
		agreements.push_back(newAgreement);
	});
	registerKeyword("dependency", [this](std::istream& theStream) {
		const EU4Agreement newAgreement(theStream);
		agreements.push_back(newAgreement);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
