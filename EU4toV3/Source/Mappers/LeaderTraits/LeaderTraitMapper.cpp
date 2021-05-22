#include "LeaderTraitMapper.h"
#include "LeaderTraitTypes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::LeaderTraitMapper::LeaderTraitMapper()
{
	LOG(LogLevel::Info) << "Loading leader traits.";
	registerKeys();
	parseFile("configurables/leader_traits.txt");
	clearRegisteredKeywords();
}

mappers::LeaderTraitMapper::LeaderTraitMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::LeaderTraitMapper::registerKeys()
{
	registerKeyword("personality", [this](const std::string& unused, std::istream& theStream) {
		const LeaderTraitTypes traitBlock(theStream);
		personalities = traitBlock.getTraits();
	});
	registerKeyword("background", [this](const std::string& unused, std::istream& theStream) {
		const LeaderTraitTypes traitBlock(theStream);
		backgrounds = traitBlock.getTraits();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::LeaderTraitMapper::getBackground(const int fire, const int shock, const int maneuver, const int siege) const
{
	for (const auto& trait: backgrounds)
	{
		if (trait.matches(fire, shock, maneuver, siege))
			return trait.getName();
	}
	return std::nullopt;
}

std::optional<std::string> mappers::LeaderTraitMapper::getPersonality(const int fire, const int shock, const int maneuver, const int siege) const
{
	for (const auto& trait: personalities)
	{
		if (trait.matches(fire, shock, maneuver, siege))
			return trait.getName();
	}
	return std::nullopt;
}
