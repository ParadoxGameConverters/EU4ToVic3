#include "ColonialTagMapping.h"
#include "ClayManager/ClayManager.h"
#include "ClayManager/State/SubState.h"
#include "ColonialRegionMapper/ColonialRegionMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"

mappers::ColonialTagMapping::ColonialTagMapping(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ColonialTagMapping::registerKeys()
{
	registerKeyword("region", [this](std::istream& theStream) {
		region = commonItems::getString(theStream);
	});
	registerKeyword("alone", [this](std::istream& theStream) {
		aloneName = commonItems::getString(theStream);
	});
	registerKeyword("tag", [this](std::istream& theStream) {
		tag = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ColonialTagMapping::matchColonialTag(const V3::Country& country,
	 const ColonialRegionMapper& colonialRegionMapper,
	 const V3::ClayManager& clayManager) const
{
	const auto& subStates = country.getSubStates();
	if (subStates.empty())
		return std::nullopt;

	const auto& capitalStateName = country.getProcessedData().capitalStateName;
	if (capitalStateName.empty() && !tag.empty())
		return std::nullopt;

	// first the capital matches
	if (!tag.empty())
	{
		if (capitalStateName == region)
			return tag;
		return std::nullopt;
	}

	if (aloneName.empty())
	{
		Log(LogLevel::Warning) << "There is an error with " << region << " mapping in colonial_tags.txt. No alone entry.";
		return std::nullopt;
	}

	// then the whole region match.
	const auto& colonialRegions = colonialRegionMapper.getColonialRegions();
	if (!colonialRegions.contains(region))
	{
		Log(LogLevel::Warning) << "Attempting to colonial region match " << region << " which isn't defined in colonial_regions.txt!";
		return std::nullopt;
	}

	// what is required
	const auto& requiredRegions = colonialRegions.at(region).getRegions();
	std::set<std::string> requiredStates;
	for (const auto& theRegion: requiredRegions)
	{
		auto states = clayManager.getStateNamesForRegion(theRegion);
		requiredStates.insert(states.begin(), states.end());
	}

	// what is owned
	std::set<std::string> ownedStates;
	for (const auto& state: country.getSubStates())
		ownedStates.emplace(state->getHomeStateName());

	// do we have presence in each of the required states? No need to check for 100% ownership, presence is sufficient.
	for (const auto& requiredState: requiredStates)
		if (!ownedStates.contains(requiredState))
			return std::nullopt;

	return aloneName;
}
