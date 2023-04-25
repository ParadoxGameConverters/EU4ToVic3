#ifndef CULTURE_MAPPER_H
#define CULTURE_MAPPER_H
#include "ColonialRegionMapper/ColonialRegionMapper.h"
#include "CultureDefinitionLoader/CultureDef.h"
#include "CultureMappingRule.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "TraitDefinitionLoader/TraitDef.h"
#include "WesternizationMapper/WesternizationMapper.h"

namespace EU4
{
class CultureLoader;
class ReligionLoader;
class CultureGroupParser;
class CultureParser;
class EU4LocalizationLoader;
} // namespace EU4

namespace V3
{
class ClayManager;
}
namespace mappers
{
class NameListLoader;
class NameListMapper;
class NameListEntry;
class CultureTraitMapper;
class CultureDefinitionLoader;
class CultureMapper: commonItems::parser
{
  public:
	CultureMapper() = default;

	void loadMappingRules(std::istream& theStream);
	void loadMappingRules(const std::string& fileName);
	void loadColonialRules(std::istream& theStream);
	void loadColonialRules(const std::string& fileName);
	void loadWesternizationRules(std::istream& theStream);
	void loadWesternizationRules(const std::string& fileName);
	void expandCulturalMappings(const V3::ClayManager& clayManager, const EU4::CultureLoader& cultureLoader, const EU4::ReligionLoader& religionLoader);

	[[nodiscard]] const auto& getMacros() const { return encounteredMacros; }
	[[nodiscard]] const auto& getUnMappedCultures() const { return unmappedCultures; }
	[[nodiscard]] const auto& getUsedCultures() const { return usedCultures; }
	[[nodiscard]] const auto& getV3CultureDefinitions() const { return v3CultureDefinitions; }
	void setV3CultureDefinitions(const std::map<std::string, CultureDef>& culDefs) { v3CultureDefinitions = culDefs; }
	[[nodiscard]] const auto& getRelatedCultures() const { return relatedCultures; }
	[[nodiscard]] const auto& getColonialRegionMapper() const { return colonialRegionMapper; }

	[[nodiscard]] std::optional<std::string> cultureMatch(const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 const std::string& v3state,
		 const std::string& v3ownerTag,
		 bool neoCultureRequest = false,
		 bool silent = false);

	// This is a check where we ping for a v3 culture without having a clue what we're asking for exactly.
	// 1. If the state is colonial, and we have a record of that eu4 culture having a neoculture in that colony,
	//	then we return that neoculture.
	// 2. If the state is colonial, and we don't have a record of eu4 culture having a neoculture in that colony,
	//	it's possible we're a native country, and return regular match.
	// 3. If the state is not colonial, return regular match same as 2.
	[[nodiscard]] std::optional<std::string> suspiciousCultureMatch(const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 const std::string& v3state,
		 const std::string& v3ownerTag);

	void generateCultureDefinitions(const std::string& nameListsPath,
		 const std::string& nameListMapPath,
		 const std::string& cultureTraitsPath,
		 const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const EU4::EU4LocalizationLoader& eu4Locs);

	void loadCultureDefinitions(const commonItems::ModFilesystem& modFS);
	void loadTraitDefinitions(const commonItems::ModFilesystem& modFS);

	void injectReligionsIntoCultureDefs(const V3::ClayManager& clayManager);

	[[nodiscard]] int getWesternizationScoreForCulture(const std::string& cultureName) const;
	[[nodiscard]] int getLiteracyScoreForCulture(const std::string& cultureName) const;
	[[nodiscard]] int getIndustryScoreForCulture(const std::string& cultureName) const;

	[[nodiscard]] std::optional<bool> doCulturesShareHeritageTrait(const std::string& cultureA, const std::string& cultureB) const;
	[[nodiscard]] std::optional<bool> doCulturesShareNonHeritageTrait(const std::string& cultureA, const std::string& cultureB) const;

  private:
	[[nodiscard]] std::string getNeoCultureMatch(const std::string& eu4culture, const std::string& v3state, const V3::ClayManager& clayManager);
	[[nodiscard]] CultureDef generateCultureDefinition(const V3::ClayManager& clayManager,
		 const std::string& eu4CultureName,
		 const CultureTraitMapper& cultureTraitMapper,
		 const NameListMapper& nameListMapper,
		 const NameListLoader& nameListLoader,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const EU4::EU4LocalizationLoader& eu4Locs,
		 const std::string& seedName = {});

	void registerKeys();

	std::vector<CultureMappingRule> cultureMapRules;
	std::map<std::string, std::string> encounteredMacros;
	std::set<std::string> unmappedCultures; // same name for eu4 as for vic3.
	std::set<std::string> usedCultures;		 // Only the stuff we actually use in Vic3.
	std::map<std::string, CultureDef> v3CultureDefinitions;
	std::map<std::string, TraitDef> v3TraitDefinitions;
	std::map<std::string, std::map<std::string, std::string>> colonyNeoCultureTargets; // colony->[eu4 culture -> v3 neoculture]
	std::map<std::string, std::set<std::string>> relatedCultures; // vanilla culture -> related dynamic cultures (hungarian -> {hungaro-latvian ... })

	ColonialRegionMapper colonialRegionMapper;
	WesternizationMapper westernizationMapper;
};
} // namespace mappers

#endif // CULTURE_MAPPER_H