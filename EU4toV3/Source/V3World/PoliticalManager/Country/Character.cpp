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
	std::ranges::replace(toReturn.begin(), toReturn.end(), ' ', '_');
	std::ranges::replace(toReturn.begin(), toReturn.end(), '\'', '_');
	return toReturn;
}
} // namespace

V3::Character::Character(const EU4::Character& character,
	 const mappers::CharacterTraitMapper& characterTraitMapper,
	 int ageShift,
	 const ClayManager& clayManager,
	 mappers::CultureMapper& cultureMapper,
	 const mappers::ReligionMapper& religionMapper,
	 const EU4::CultureLoader& cultureLoader,
	 const EU4::ReligionLoader& religionLoader,
	 const std::string& capitalStateName,
	 const std::string& tag)
{
	// name!
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

		utf8 = commonItems::convertWin1252ToUTF8(character.dynasty);
		lastName = normalizeString(utf8);
		localizations.emplace(lastName, utf8);
	}

	// rulership
	if (character.ruler)
		ruler = true;
	else if (character.heir)
		heir = true;

	if (character.female)
		female = true;

	// leadership
	if (!character.leaderType.empty())
	{
		if (character.leaderType == "general" || character.leaderType == "conquistador")
			general = true;
		else if (character.leaderType == "admiral" || character.leaderType == "explorer")
			admiral = true;

		// rank
		const auto rank = static_cast<int>(std::ceil(static_cast<double>(character.fire + character.shock + character.fire + character.siege) / 4.0));
		commanderRank = "commander_rank_" + std::to_string(rank);
		if (!character.leaderTrait.empty())
		{
			if (const auto& personalityMatch = characterTraitMapper.getPersonality(character.leaderTrait); personalityMatch)
				traits.emplace(*personalityMatch);
		}
	}

	// dates
	birthDate = character.birthDate;
	birthDate.ChangeByYears(ageShift);

	// usually rulers have up to three personality traits. We can't map all of them, so pick just the first.
	if (!character.traits.empty())
	{
		if (const auto& personalityMatch = characterTraitMapper.getPersonality(*character.traits.begin()); personalityMatch)
			traits.emplace(*personalityMatch);
	}

	// get some skilltrait.
	if (const auto& skillTraits = characterTraitMapper.getSkillTraits(character); !skillTraits.empty())
		traits.insert(skillTraits.begin(), skillTraits.end());

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
