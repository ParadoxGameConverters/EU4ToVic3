#include "Character.h"
#include "CharacterTraitMapper/CharacterTraitMapper.h"
#include "CultureMapper/CultureMapper.h"
#include "OSCompatibilityLayer.h"
#include "ReligionMapper/ReligionMapper.h"
#include <algorithm>
#include <cmath>

namespace
{
std::string normalizeString(const std::string& input)
{
	auto toReturn = commonItems::normalizeUTF8Path(input);
	std::ranges::replace(toReturn, ' ', '_');
	std::ranges::replace(toReturn, '\'', '_');
	std::ranges::replace(toReturn, '(', '_');
	std::ranges::replace(toReturn, ')', '_');
	return toReturn;
}
} // namespace

V3::Character::Character(const EU4::Character& character,
	 const mappers::CharacterTraitMapper& characterTraitMapper,
	 float ageShift,
	 const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& capitalStateName,
	 const std::string& tag,
	 const date& conversionDate)
{
	// name!
	convertName(character);
	if (firstName.empty())
	{
		// We CAN get people without a normalizable name. Mods and whatnot. Not my problem.
		if (!character.female)
			firstName = "Bob";
		else
			firstName = "Bobbina";
		localizations.emplace(firstName, firstName);
		if (lastName.empty())
		{
			lastName = "BobbyPants";
			localizations.emplace(lastName, lastName);
		}
	}

	// rulership
	convertRulership(character);

	// leadership
	convertLeadership(character, characterTraitMapper);

	// dates
	convertAge(character, ageShift, conversionDate);

	// traits
	convertTraits(character, characterTraitMapper);

	// are we dry and incompetent?
	if (isCharacterDryAndIncompetent(character))
	{
		const int seed = static_cast<int>(firstName[0]) * 17 + 11;
		traits.emplace(characterTraitMapper.getGratisIncompetency(seed));
	}

	// are we a leader that's semi-dry and hypercompetent?
	if (isLeaderSemiDryAndHyperCompetent(character))
	{
		const int seed = static_cast<int>(firstName[0]) * 14 + 82;
		traits.emplace(characterTraitMapper.getGratisVeterancy(seed));
	}

	// are we semi-dry and OLD?
	if (isCharacterSemiDryAndOld())
	{
		const int seed = static_cast<int>(firstName[0]) * 11 + 51;
		traits.emplace(characterTraitMapper.getGratisAgeism(seed));
	}

	// Are we young?
	if (age < 15)
		traits.emplace("trait_child");

	// are we *still* dry?
	if (traits.empty())
	{
		const int seed = static_cast<int>(firstName[0]) * 19 + 31;
		if (seed % 3 == 0)
			traits.emplace(characterTraitMapper.getGratisDisorder(seed));
	}

	// culture & religion
	if (!character.culture.empty())
	{
		if (const auto& cultureMatch =
				  cultureMapper.suspiciousCultureMatch(clayManager, cultureLoader, religionLoader, character.culture, character.religion, capitalStateName, tag);
			 cultureMatch)
			culture = *cultureMatch;
	}
	if (!character.religion.empty())
	{
		if (const auto& religionMatch = religionMapper.getV3Religion(character.religion); religionMatch)
			religion = *religionMatch;
	}

	// rest is up to country.
}

void V3::Character::convertName(const EU4::Character& character)
{
	if (character.name.empty())
	{
		// this is a leader. It only has a composite name which we would need to .. break at a sane position. Yeah.
		// Need to .. uhh.. dammit.
		const auto utf8 = commonItems::convertWin1252ToUTF8(character.leaderName);
		firstName = normalizeString(utf8);
		localizations.emplace(firstName, utf8);
	}
	else
	{
		// maybe better name?
		std::string utf8;
		if (character.ruler && !character.futureMonarchName.empty())
			utf8 = commonItems::convertWin1252ToUTF8(character.futureMonarchName);
		else
			utf8 = commonItems::convertWin1252ToUTF8(character.name);
		firstName = normalizeString(utf8);
		localizations.emplace(firstName, utf8);

		if (!character.dynasty.empty())
		{
			utf8 = commonItems::convertWin1252ToUTF8(character.dynasty);
			lastName = normalizeString(utf8);
			localizations.emplace(lastName, utf8);
		}
	}
}

void V3::Character::convertRulership(const EU4::Character& character)
{
	if (character.ruler)
		ruler = true;
	else if (character.heir)
		heir = true;

	// eh. rulership.
	if (character.female)
		female = true;
}

void V3::Character::convertLeadership(const EU4::Character& character, const mappers::CharacterTraitMapper& characterTraitMapper)
{
	if (!character.leaderType.empty())
	{
		if (character.leaderType == "general" || character.leaderType == "conquistador")
			general = true;
		else if (character.leaderType == "admiral" || character.leaderType == "explorer")
			admiral = true;

		// rank
		const auto rank = static_cast<int>(std::round(static_cast<double>(character.fire + character.shock + character.fire + character.siege) / 4.8));
		if (rank > 0)
			commanderRank = "commander_rank_" + std::to_string(rank);

		if (!character.leaderTrait.empty())
		{
			if (const auto& personalityMatch = characterTraitMapper.getPersonality(character.leaderTrait); personalityMatch)
				traits.emplace(*personalityMatch);
		}
	}
}

void V3::Character::convertAge(const EU4::Character& character, float ageShift, const date& conversionDate)
{
	birthDate = character.birthDate;
	age = static_cast<int>(std::round(birthDate.diffInYears(conversionDate)));

	birthDate.ChangeByYears(static_cast<int>(std::round(ageShift)));
}

void V3::Character::convertTraits(const EU4::Character& character, const mappers::CharacterTraitMapper& characterTraitMapper)
{
	// usually rulers have up to three personality traits. We can't map all of them, so pick just the first.
	if (!character.traits.empty())
	{
		if (const auto& personalityMatch = characterTraitMapper.getPersonality(*character.traits.begin()); personalityMatch)
			traits.emplace(*personalityMatch);
	}

	// get some skilltrait.
	if (const auto& skillTraits = characterTraitMapper.getSkillTraits(character); !skillTraits.empty())
		traits.insert(skillTraits.begin(), skillTraits.end());
}

bool V3::Character::isCharacterDryAndIncompetent(const EU4::Character& character) const
{
	if (!traits.empty())
		return false;

	if (character.ruler || character.heir || character.consort)
	{
		const auto sum = character.adm + character.dip + character.mil;
		if (sum <= 4)
			return true;
	}

	if (!character.leaderType.empty())
	{
		const auto sum = character.fire + character.shock + character.maneuver + character.siege;
		if (sum <= 5)
			return true;
	}

	return false;
}

bool V3::Character::isLeaderSemiDryAndHyperCompetent(const EU4::Character& character) const
{
	if (traits.size() > 1)
		return false;
	if (character.leaderType.empty())
		return false;
	const auto sum = character.fire + character.shock + character.maneuver + character.siege;
	if (sum >= 22)
		return true;
	return false;
}

bool V3::Character::isCharacterSemiDryAndOld() const
{
	if (traits.size() > 1)
		return false;
	if (age > 70)
		return true;
	return false;
}
