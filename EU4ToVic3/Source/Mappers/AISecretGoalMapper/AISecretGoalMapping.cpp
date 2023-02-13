#include "AISecretGoalMapping.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/SubState.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
#include "Loaders/SuperRegionLoader/V3SuperRegion.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"

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
	registerKeyword("target_is_rival", [this](std::istream& theStream) {
		targetRival = commonItems::getString(theStream) == "yes";
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

bool mappers::AISecretGoalMapping::matchGoal(const V3::Country& country, const V3::Country& target, const V3::ClayManager& clayManager) const
{
	const std::map<std::string, int> tierRanks =
		 {{"city_state", 1}, {"principality", 2}, {"grand_principality", 3}, {"kingdom", 4}, {"empire", 5}, {"hegemony", 6}};
	const std::map<std::string, std::string> govForms = {{"law_chiefdom", "tribe"},
		 {"law_monarchy", "monarchy"},
		 {"law_theocracy", "theocracy"},
		 {"law_presidential_republic", "republic"},
		 {"law_parliamentary_republic", "republic"},
		 {"law_council_republic", "republic"}};

	if (capital)
	{
		if (country.getProcessedData().capitalStateName.empty())
			return false;
		if (!clayManager.stateIsInRegion(country.getProcessedData().capitalStateName, *capital))
			return false;
	}

	if (targetCapital)
	{
		if (target.getProcessedData().capitalStateName.empty())
			return false;
		if (!clayManager.stateIsInRegion(target.getProcessedData().capitalStateName, *targetCapital))
			return false;
	}

	if (targetCapitalDiffRegion)
	{
		if (country.getProcessedData().capitalStateName.empty() || target.getProcessedData().capitalStateName.empty())
			return false;
		if (const auto& capCountry = clayManager.getParentSuperRegion(country.getProcessedData().capitalStateName); capCountry)
		{
			if (const auto& capTarget = clayManager.getParentSuperRegion(target.getProcessedData().capitalStateName); capTarget)
			{
				if ((capCountry->getName() != capTarget->getName()) != *targetCapitalDiffRegion)
					return false;
			}
			else
				return false;
		}
		else
			return false;
	}

	if (targetRankLEQ)
	{
		if (tierRanks.contains(country.getProcessedData().tier) && tierRanks.contains(target.getProcessedData().tier))
		{
			if (tierRanks.at(target.getProcessedData().tier) - tierRanks.at(country.getProcessedData().tier) > *targetRankLEQ)
				return false;
		}
		else
			return false;
	}

	if (targetRankGEQ)
	{
		if (tierRanks.contains(country.getProcessedData().tier) && tierRanks.contains(target.getProcessedData().tier))
		{
			if (tierRanks.at(target.getProcessedData().tier) - tierRanks.at(country.getProcessedData().tier) < *targetRankGEQ)
				return false;
		}
		else
			return false;
	}

	if (targetSubject)
	{
		if (target.getProcessedData().overlordTag.empty())
			return false;
		if ((target.getProcessedData().overlordTag == country.getTag()) != *targetSubject)
			return false;
	}

	if (targetOverlord)
	{
		if (country.getProcessedData().overlordTag.empty())
			return false;
		if ((country.getProcessedData().overlordTag == target.getTag()) != *targetOverlord)
			return false;
	}

	if (targetRival)
	{
		if ((country.getProcessedData().rivals.contains(target.getTag())) != *targetRival)
			return false;
	}

	if (targetGP)
	{
		if (!target.getSourceCountry())
			return false;
		if (target.getSourceCountry()->isGP() != *targetGP)
			return false;
	}

	if (gp)
	{
		if (!country.getSourceCountry())
			return false;
		if (country.getSourceCountry()->isGP() != *gp)
			return false;
	}

	if (targetIsClaimed)
	{
		bool claimExists = false;
		for (const auto& subState: target.getSubStates())
			if (subState->getClaims().contains(country.getTag()))
			{
				claimExists = true;
				break;
			}
		if (claimExists != *targetIsClaimed)
			return false;
	}

	if (targetIsClaimedByRival)
	{
		bool claimExists = false;
		const auto& rivals = country.getProcessedData().rivals;
		for (const auto& subState: target.getSubStates())
		{
			for (const auto& claim: subState->getClaims())
				if (rivals.contains(claim))
				{
					claimExists = true;
					break;
				}
			if (claimExists)
				break;
		}
		if (claimExists != *targetIsClaimedByRival)
			return false;
	}

	if (govFormDiff)
	{
		std::string countryForm;
		std::string targetForm;

		for (const auto& law: country.getProcessedData().laws)
			if (govForms.contains(law))
			{
				countryForm = govForms.at(law);
				break;
			}
		for (const auto& law: target.getProcessedData().laws)
			if (govForms.contains(law))
			{
				targetForm = govForms.at(law);
				break;
			}

		if (countryForm.empty() || targetForm.empty())
			return false;

		if ((countryForm != targetForm) != *govFormDiff)
			return false;
	}

	return true;
}
