#include "CharacterTraitMapper.h"
#include "AssignmentMapping.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <ranges>

#include "CountryManager/CountryLeader/EU4Character.h"

void mappers::CharacterTraitMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading character trait mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
}

void mappers::CharacterTraitMapper::registerKeys()
{
	registerKeyword("personalities", [this](std::istream& theStream) {
		personalities = AssignmentMapping(theStream).getAssignments();
	});
	registerKeyword("global", [this](std::istream& theStream) {
		global = AssignmentMapping(theStream).getAssignments();
	});
	registerKeyword("admiral", [this](std::istream& theStream) {
		admirals.emplace_back(SkillMapping(theStream));
	});
	registerKeyword("general", [this](std::istream& theStream) {
		generals.emplace_back(SkillMapping(theStream));
	});
	registerKeyword("ruler", [this](std::istream& theStream) {
		rulers.emplace_back(SkillMapping(theStream));
	});
	registerKeyword("incompetency", [this](std::istream& theStream) {
		const auto incompetencyList = commonItems::getStrings(theStream);
		incompetencies.insert(incompetencyList.begin(), incompetencyList.end());
	});
	registerKeyword("veterancy", [this](std::istream& theStream) {
		const auto veterancyList = commonItems::getStrings(theStream);
		veterancies.insert(veterancyList.begin(), veterancyList.end());
	});
	registerKeyword("age", [this](std::istream& theStream) {
		const auto ageList = commonItems::getStrings(theStream);
		ageisms.insert(ageList.begin(), ageList.end());
	});
	registerKeyword("disorders", [this](std::istream& theStream) {
		const auto disorderList = commonItems::getStrings(theStream);
		disorders.insert(disorderList.begin(), disorderList.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::CharacterTraitMapper::getPersonality(const std::string& eu4Personality) const
{
	if (personalities.contains(eu4Personality))
		return personalities.at(eu4Personality);
	return std::nullopt;
}

std::set<std::string> mappers::CharacterTraitMapper::getSkillTraits(const EU4::Character& eu4Character) const
{
	// map general/admiral skill trait.
	std::set<std::string> traits;
	if (!eu4Character.leaderType.empty())
	{
		if (global.contains(eu4Character.leaderType))
			traits.emplace(global.at(eu4Character.leaderType));
		else if (const auto& traitMatch = matchLeaderSkill(eu4Character.leaderType,
						 std::vector<int>{eu4Character.fire, eu4Character.shock, eu4Character.maneuver, eu4Character.siege});
					traitMatch)
			traits.emplace(*traitMatch);
	}
	// map ruler trait
	if (eu4Character.ruler || eu4Character.consort || eu4Character.heir)
	{
		if (const auto& traitMatch = matchLeaderSkill("ruler", std::vector<int>{eu4Character.adm, eu4Character.dip, eu4Character.mil}); traitMatch)
			traits.emplace(*traitMatch);
	}

	return traits;
}

std::optional<std::string> mappers::CharacterTraitMapper::matchLeaderSkill(const std::string& leaderType, const std::vector<int>& skills) const
{
	if (leaderType == "general")
		for (const auto& mapping: generals)
			if (const auto& match = mapping.matchTrait(skills); match)
				return match;
	if (leaderType == "admiral")
		for (const auto& mapping: admirals)
			if (const auto& match = mapping.matchTrait(skills); match)
				return match;
	if (leaderType == "ruler")
		for (const auto& mapping: rulers)
			if (const auto& match = mapping.matchTrait(skills); match)
				return match;
	return std::nullopt;
}

std::string mappers::CharacterTraitMapper::getGratisIncompetency(int seed) const
{
	auto pos = seed;
	if (pos < 0)
		pos = 0;
	if (pos >= static_cast<int>(incompetencies.size()))
		pos = pos % static_cast<int>(incompetencies.size());
	return *std::next(incompetencies.begin(), pos);
}

std::string mappers::CharacterTraitMapper::getGratisVeterancy(int seed) const
{
	auto pos = seed;
	if (pos < 0)
		pos = 0;
	if (pos >= static_cast<int>(veterancies.size()))
		pos = pos % static_cast<int>(veterancies.size());
	return *std::next(veterancies.begin(), pos);
}

std::string mappers::CharacterTraitMapper::getGratisAgeism(int seed) const
{
	auto pos = seed;
	if (pos < 0)
		pos = 0;
	if (pos >= static_cast<int>(ageisms.size()))
		pos = pos % static_cast<int>(ageisms.size());
	return *std::next(ageisms.begin(), pos);
}

std::string mappers::CharacterTraitMapper::getGratisDisorder(int seed) const
{
	auto pos = seed;
	if (pos < 0)
		pos = 0;
	if (pos >= static_cast<int>(disorders.size()))
		pos = pos % static_cast<int>(disorders.size());
	return *std::next(disorders.begin(), pos);
}
