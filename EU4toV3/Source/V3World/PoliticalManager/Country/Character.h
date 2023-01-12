#ifndef CHARACTER_H
#define CHARACTER_H
#include "CountryManager/CountryLeader/EU4Character.h"
#include "Date.h"
#include "Loaders/LocLoader/LocalizationLoader.h"
#include <set>
#include <string>

namespace EU4
{
class CultureLoader;
class ReligionLoader;
} // namespace EU4
namespace mappers
{
class CharacterTraitMapper;
class CultureMapper;
class ReligionMapper;
} // namespace mappers
namespace V3
{
class ClayManager;
struct Character
{
	Character() = default;
	explicit Character(const EU4::Character& character,
		 const mappers::CharacterTraitMapper& characterTraitMapper,
		 float ageShift,
		 const ClayManager& clayManager,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& capitalStateName,
		 const std::string& tag,
		 const date& conversionDate);

	void convertName(const EU4::Character& character);
	void convertRulership(const EU4::Character& character);
	void convertLeadership(const EU4::Character& character, const mappers::CharacterTraitMapper& characterTraitMapper);
	void convertAge(const EU4::Character& character, float ageShift, const date& conversionDate);
	void convertTraits(const EU4::Character& character, const mappers::CharacterTraitMapper& characterTraitMapper);
	[[nodiscard]] bool isCharacterDryAndIncompetent(const EU4::Character& character) const;
	[[nodiscard]] bool isLeaderSemiDryAndHyperCompetent(const EU4::Character& character) const;
	[[nodiscard]] bool isCharacterSemiDryAndOld() const;

	std::string firstName; // KEY
	std::string lastName;  // KEY
	bool ruler = false;
	bool igLeader = false;
	bool noble = false; // ??
	bool heir = false;
	bool admiral = false;
	bool general = false;
	std::string hq;				// leave dry for now. Or set to capital.
	std::string commanderRank; // key!
	bool married = false;		// set from outside.
	bool female = false;

	int age = 0;			 // if unchanged, won't get exported.
	std::string culture;	 // culture = cu:culture !
	std::string religion; // religion = rel:religion !
	date birthDate;
	std::string dnaKey; // unset for now. Can't map this easily.
	std::string interestGroup;
	std::string ideology;
	std::set<std::string> traits;
	std::map<std::string, std::string> localizations; // coming from save, this is all english. Also UTF8ied.
};
} // namespace V3

#endif // CHARACTER_H