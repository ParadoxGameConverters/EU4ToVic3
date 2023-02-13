#include "AISecretGoalMapping.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

mappers::AISecretGoalMapping::AISecretGoalMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::AISecretGoalMapping::registerKeys()
{
	registerKeyword("capital", [this](std::istream& theStream) {
		capital = commonItems::getString(theStream);
	});
	registerKeyword("target_capital", [this](std::istream& theStream) {
		targetCapital = commonItems::getString(theStream);
	});
	registerKeyword("target_capital_diff_region", [this](std::istream& theStream) {
		targetCapitalDiffRegion = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("target_rank_leq", [this](std::istream& theStream) {
		targetRankLEQ = commonItems::getInt(theStream);
	});
	registerKeyword("target_rank_geq", [this](std::istream& theStream) {
		targetRankGEQ = commonItems::getInt(theStream);
	});
	registerKeyword("target_subject", [this](std::istream& theStream) {
		targetSubject = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("target_overlord", [this](std::istream& theStream) {
		targetOverlord = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("target_gp", [this](std::istream& theStream) {
		targetGP = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("gp", [this](std::istream& theStream) {
		gp = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("target_is_claimed", [this](std::istream& theStream) {
		targetIsClaimed = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("target_is_claimed_by_rival", [this](std::istream& theStream) {
		targetIsClaimedByRival = commonItems::getString(theStream) == "yes";
	});
	registerKeyword("gov_form_diff", [this](std::istream& theStream) {
		govFormDiff = commonItems::getString(theStream) == "yes";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
