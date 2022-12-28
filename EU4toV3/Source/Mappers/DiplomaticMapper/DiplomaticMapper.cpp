#include "DiplomaticMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::DiplomaticMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing Diplomatic Mapping Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
}

void mappers::DiplomaticMapper::registerKeys()
{
	registerKeyword("dominion", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		dominions.insert(theList.begin(), theList.end());
	});
	registerKeyword("protectorate", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		protectorates.insert(theList.begin(), theList.end());
	});
	registerKeyword("defensive_pact", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		defensivePacts.insert(theList.begin(), theList.end());
	});
	registerKeyword("tributary", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		tributaries.insert(theList.begin(), theList.end());
	});
	registerKeyword("personal_union", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		personalUnions.insert(theList.begin(), theList.end());
	});
	registerKeyword("puppet", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		puppets.insert(theList.begin(), theList.end());
	});
	registerKeyword("vassal", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		vassals.insert(theList.begin(), theList.end());
	});
	registerKeyword("trade_agreement", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		tradeAgreements.insert(theList.begin(), theList.end());
	});
	registerKeyword("customs_union", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		customsUnions.insert(theList.begin(), theList.end());
	});
	registerKeyword("double_relationship_boost", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		doubleRelationshipBoosts.insert(theList.begin(), theList.end());
	});
	registerKeyword("double_defensive_pact", [this](std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		doubleDefensivePacts.insert(theList.begin(), theList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
