#include "EU4Leader.h"
#include "CommonRegexes.h"
#include "EU4LeaderID.h"
#include "ParserHelpers.h"

void EU4::Leader::parseLeader(std::istream& theStream)
{
	registerKeys();
	leaderParser.parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::Leader::parseRuler(std::istream& theStream)
{
	registerKeys();
	rulerParser.parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::Leader::registerKeys()
{
	leaderParser.registerSetter("name", character.leaderName);
	leaderParser.registerSetter("type", character.leaderType);
	leaderParser.registerKeyword("female", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		character.female = true;
	});
	leaderParser.registerSetter("manuever", character.maneuver);
	leaderParser.registerSetter("fire", character.fire);
	leaderParser.registerSetter("shock", character.shock);
	leaderParser.registerSetter("siege", character.siege);
	leaderParser.registerKeyword("activation", [this](std::istream& theStream) {
		character.activationDate = date(commonItems::getString(theStream));
	});
	leaderParser.registerKeyword("birth_date", [this](std::istream& theStream) {
		character.birthDate = date(commonItems::getString(theStream));
	});
	leaderParser.registerKeyword("id", [this](std::istream& theStream) {
		const LeaderID theID(theStream);
		character.leaderID = theID.getIDNum();
	});
	leaderParser.registerKeyword("monarch_id", [this](std::istream& theStream) {
		const LeaderID theID(theStream);
		character.monarchID = theID.getIDNum();
	});
	leaderParser.registerSetter("personality", character.leaderTrait);
	leaderParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	rulerParser.registerSetter("name", character.name);
	rulerParser.registerSetter("country", character.country);
	rulerParser.registerSetter("ADM", character.adm);
	rulerParser.registerSetter("DIP", character.dip);
	rulerParser.registerSetter("MIL", character.mil);
	rulerParser.registerSetter("culture", character.culture);
	rulerParser.registerSetter("religion", character.religion);
	rulerParser.registerKeyword("personalities", [this](std::istream& theStream) {
		const auto keys = commonItems::stringsOfItemNames(theStream).getStrings();
		character.traits.insert(keys.begin(), keys.end());
	});
	rulerParser.registerKeyword("leader_id", [this](std::istream& theStream) {
		const LeaderID theID(theStream);
		character.leaderID = theID.getIDNum();
	});
	rulerParser.registerKeyword("id", [this](std::istream& theStream) {
		const LeaderID theID(theStream);
		character.monarchID = theID.getIDNum();
	});
	rulerParser.registerSetter("dynasty", character.dynasty);
	rulerParser.registerKeyword("birth_date", [this](std::istream& theStream) {
		character.birthDate = date(commonItems::getString(theStream));
	});
	rulerParser.registerKeyword("female", [this](std::istream& theStream) {
		commonItems::ignoreItem("unused", theStream);
		character.female = true;
	});
	rulerParser.registerSetter("monarch_name", character.futureMonarchName);
	rulerParser.registerSetter("claim", character.heirClaim);
	rulerParser.registerSetter("country_of_origin", character.countryOfOrigin);
	rulerParser.registerSetter("culture", character.culture);
	rulerParser.registerSetter("religion", character.religion);
	rulerParser.registerKeyword("leader", [this](std::istream& theStream) {
		leaderParser.parseStream(theStream);
	});
	rulerParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
