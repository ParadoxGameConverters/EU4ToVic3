#ifndef IG_IDEOLOGIES_MAPPING_H
#define IG_IDEOLOGIES_MAPPING_H
#include "Parser.h"

namespace V3
{
class Country;
class ClayManager;
} // namespace V3

namespace mappers
{
class CultureMapper;
class ReligionMapper;
struct IGIdeologyMod
{
	std::set<std::string> addedIdeologies;
	std::set<std::string> removedIdeologies;
};
class IGIdeologiesMapping: commonItems::parser
{
  public:
	IGIdeologiesMapping() = default;
	explicit IGIdeologiesMapping(std::istream& theStream);

	[[nodiscard]] std::map<std::string, IGIdeologyMod> alterIdeologyMods(const std::map<std::string, IGIdeologyMod>& incMods,
		 const V3::Country& country,
		 const CultureMapper& cultureMapper,
		 const ReligionMapper& religionMapper,
		 const V3::ClayManager& clayManager) const;

  private:
	void registerKeys();

	[[nodiscard]] bool matchCountry(const V3::Country& country,
		 const CultureMapper& cultureMapper,
		 const ReligionMapper& religionMapper,
		 const V3::ClayManager& clayManager) const;

	std::set<std::string> cultureTraits;
	std::set<std::string> religionTraits;
	std::set<std::string> eu4Religions;
	std::set<std::string> eu4Govs;
	std::set<std::string> eu4Ideas;
	std::set<std::string> eu4Reforms;
	std::set<std::string> capitals;
	std::set<std::string> laws;
	std::optional<bool> colony;
	std::optional<bool> emperorOfChina;
	std::optional<bool> HREmperor;
	std::map<std::string, IGIdeologyMod> IGIdeologyMods; // ig, added and removed ideologies.
};
} // namespace mappers

#endif // IG_IDEOLOGIES_MAPPING_H