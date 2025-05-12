#include "LawMapper.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
#include "LawGroup.h"
#include "Log.h"
#include "PoliticalManager/Country/Country.h"
#include <algorithm>
#include <ranges>

namespace
{
std::vector<std::string> sortLaws(const std::map<std::string, std::pair<double, double>>& theMap)
{
	std::vector<std::string> sorted;

	std::vector<std::pair<std::string, std::pair<double, double>>> pairs;
	for (const auto& theElement: theMap)
		pairs.emplace_back(theElement);

	std::sort(pairs.begin(),
		 pairs.end(),
		 [=](const std::pair<std::string, std::pair<double, double>>& a, const std::pair<std::string, std::pair<double, double>>& b) {
			 if (a.second.first > b.second.first)
				 return true;
			 if (b.second.first > a.second.first)
				 return false;
			 return a.second.second > b.second.second;
		 });

	for (const auto& key: pairs | std::views::keys)
		sorted.emplace_back(key);

	return sorted;
}
} // namespace


void mappers::LawMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading Law Mapping Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << lawGroups.size() << " law groups loaded.";
}

void mappers::LawMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& lawGroupName, std::istream& theStream) {
		lawGroups.emplace(lawGroupName, LawGroup(theStream).getLaws());
	});
}

std::optional<std::string> mappers::LawMapper::grantLawFromGroup(const std::string& lawGroup, const V3::Country& country) const
{
	if (!lawGroups.contains(lawGroup) || !country.getSourceCountry() || country.getSourceCountry()->getGovernment().empty())
		return std::nullopt;

	// prep all needed.
	const auto& form = country.getSourceCountry()->getGovernment();
	const auto& ideas = country.getSourceCountry()->getNationalIdeas();
	const auto& reforms = country.getSourceCountry()->getReforms();
	const auto& existingLaws = country.getProcessedData().laws;
	const auto& existingTechs = country.getProcessedData().techs;

	std::map<std::string, std::pair<double, double>> lawScores; // how strong selections are these laws. Law -> pair<score, progressiveness>.
	std::set<std::string> nonBlockedLaws;								// these laws are *not* a match but are not blocked. Used for fallbacks.

	// Roll through all laws and look for all matches - how well they fit.
	for (const auto& [lawName, lawMapping]: lawGroups.at(lawGroup))
	{
		// do we already have a law from this group? If so bail on everything!
		if (existingLaws.contains(lawName))
			return std::nullopt;

		if (!laws.contains(lawName))
		{
			Log(LogLevel::Warning) << "We're trying to map to law: " << lawName << " that has no definition! Skipping.";
			continue;
		}

		const auto score = lawMapping.getScore(form, ideas, reforms);
		if (score > 0) // can't place laws with 0 score since they may be nominally incompatible with our country (monarchy instead of republic etc).
			lawScores.emplace(lawName, std::pair{score, laws.at(lawName).progressiveness});
		else if (score == 0.0 && !isLawBlocked(lawName, existingLaws, existingTechs))
			nonBlockedLaws.emplace(lawName); // we can use these is push comes to shove.
	}

	// then sort the lawScores, highest match first.
	const auto sortedLaws = sortLaws(lawScores);

	// iterating through these, go to first law that doesn't block and return that.
	for (const auto& lawName: sortedLaws)
	{
		if (isLawBlocked(lawName, existingLaws, existingTechs))
			continue;

		// we found a match.
		return lawName;
	}

	// failing all, well, look to "unselections" - laws that don't match at all but that aren't technically forbidden,
	// and pick the most progressive one. No worries at picking late-game laws since we never map to them in law_map.txt,
	// so they are never even considered.
	auto sortedUnSelections = filterLawsByAllowedProgressiveness(nonBlockedLaws);
	if (!sortedUnSelections.empty())
		return *sortedUnSelections.begin();

	Log(LogLevel::Debug) << "Zero law matches for " << country.getTag() << " in group: " << lawGroup;
	return std::nullopt;
}

bool mappers::LawMapper::isLawBlocked(const std::string& lawName, const std::set<std::string>& existingLaws, const std::set<std::string>& existingTechs) const
{

	const auto& law = laws.at(lawName);

	for (const auto& existingLaw: existingLaws)
		if (law.blockingLaws.contains(existingLaw))
			return true;

	if (!law.requiredLaws.empty())
	{
		const bool match = std::ranges::any_of(law.requiredLaws.begin(), law.requiredLaws.end(), [existingLaws](const auto& law) {
			return existingLaws.contains(law);
		});
		if (!match)
			return true;
	}

	if (!law.requiredTechs.empty())
	{
		const bool match = std::ranges::any_of(law.requiredTechs.begin(), law.requiredTechs.end(), [existingTechs](const auto& tech) {
			return existingTechs.contains(tech);
		});
		if (!match)
			return true;
	}

	return false;
}

std::vector<std::string> mappers::LawMapper::filterLawsByAllowedProgressiveness(const std::set<std::string>& nonBlockedLaws) const
{
	std::map<std::string, std::pair<double, double>> possibleLaws;
	for (const auto& lawName: nonBlockedLaws)
		possibleLaws.emplace(lawName, std::pair{0, laws.at(lawName).progressiveness});
	auto sortedLaws = sortLaws(possibleLaws);
	return sortedLaws;
}