#include "IdeaEffects.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::IdeaEffects::IdeaEffects(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::IdeaEffects::registerKeys()
{
	registerKeyword("enforce", [this](const std::string& unused, std::istream& theStream) {
		enforce = commonItems::singleString(theStream).getString();
	});
	registerKeyword("army", [this](const std::string& unused, std::istream& theStream) {
		army = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("navy", [this](const std::string& unused, std::istream& theStream) {
		navy = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("commerce", [this](const std::string& unused, std::istream& theStream) {
		commerce = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("culture", [this](const std::string& unused, std::istream& theStream) {
		culture = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("industry", [this](const std::string& unused, std::istream& theStream) {
		industry = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("slavery", [this](const std::string& unused, std::istream& theStream) {
		slavery = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("upper_house_composition", [this](const std::string& unused, std::istream& theStream) {
		upper_house_composition = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("vote_franchise", [this](const std::string& unused, std::istream& theStream) {
		vote_franchise = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("voting_system", [this](const std::string& unused, std::istream& theStream) {
		voting_system = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("public_meetings", [this](const std::string& unused, std::istream& theStream) {
		public_meetings = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("press_rights", [this](const std::string& unused, std::istream& theStream) {
		press_rights = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("trade_unions", [this](const std::string& unused, std::istream& theStream) {
		trade_unions = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("political_parties", [this](const std::string& unused, std::istream& theStream) {
		political_parties = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("reactionary", [this](const std::string& unused, std::istream& theStream) {
		reactionary = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("liberal", [this](const std::string& unused, std::istream& theStream) {
		liberal = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("order", [this](const std::string& unused, std::istream& theStream) {
		order = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("liberty", [this](const std::string& unused, std::istream& theStream) {
		liberty = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("equality", [this](const std::string& unused, std::istream& theStream) {
		equality = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerKeyword("literacy", [this](const std::string& unused, std::istream& theStream) {
		literacy = commonItems::singleInt(theStream).getInt() - 5;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
