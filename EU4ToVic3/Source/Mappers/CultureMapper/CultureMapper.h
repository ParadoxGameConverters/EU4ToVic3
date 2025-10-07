#ifndef CULTURE_MAPPER_H
#define CULTURE_MAPPER_H
#include "ColonialRegionMapper/ColonialRegionMapper.h"
#include "CultureDefinitionLoader/CultureDef.h"
#include "CultureMappingRule.h"
#include "ModLoader/ModFilesystem.h"
#include "NewEU4CultureMapper/NewEU4CultureMapper.h"
#include "Parser.h"
#include "TraitDefinitionLoader/TraitDef.h"
#include "WesternizationMapper/WesternizationMapper.h"
#include <filesystem>

namespace EU4
{
class Province;
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
	void loadMappingRules(const std::filesystem::path& fileName);
	void loadColonialRules(std::istream& theStream);
	void loadColonialRules(const std::filesystem::path& fileName);
	void loadWesternizationRules(std::istream& theStream);
	void loadWesternizationRules(const std::filesystem::path& fileName);
	void loadNewEU4CultureRules(std::istream& theStream);
	void loadNewEU4CultureRules(const std::filesystem::path& fileName);
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

	void generateCultureDefinitions(const std::filesystem::path& nameListsPath,
		 const std::filesystem::path& nameListMapPath,
		 const std::filesystem::path& cultureTraitsPath,
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

	[[nodiscard]] std::optional<bool> doCulturesShareHeritage(const std::string& cultureA, const std::string& cultureB) const;
	[[nodiscard]] std::optional<bool> doCulturesShareLanguage(const std::string& cultureA, const std::string& cultureB) const;
	[[nodiscard]] std::optional<bool> doCulturesShareTradition(const std::string& cultureA, const std::string& cultureB) const;

	void alterNewEU4CultureDefinitions(const std::map<int, std::shared_ptr<EU4::Province>>& provinces);
	[[nodiscard]] bool isCultureNeoCulturallyOverridden(const std::string& culture) const;

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

	void recordCultureMapping(const auto& eu4Culture, const auto& v3Culture);
	void markNeoCultureOverrides();

	void registerKeys();

	std::vector<CultureMappingRule> cultureMapRules;
	std::map<std::string, std::string> encounteredMacros;
	std::set<std::string> unmappedCultures; // same name for eu4 as for vic3.
	std::set<std::string> usedCultures;		 // Only the stuff we actually use in Vic3.
	std::map<std::string, CultureDef> v3CultureDefinitions;
	std::map<std::string, TraitDef> v3TraitDefinitions;
	std::map<std::string, std::map<std::string, std::string>> colonyNeoCultureTargets; // colony->[eu4 culture -> v3 neoculture]
	std::map<std::string, std::set<std::string>> relatedCultures; // vanilla culture -> related dynamic cultures (hungarian -> {hungaro-latvian ... })
	std::map<std::string, std::set<std::string>>
		 eu4ToVic3CultureRecord; // recording all targets a particular eu4 culture mapped to. Relevant if we're editing traits of all targets in postprocessing.
	std::set<std::string> eu4OverriddenNeoCultures; // Stuff like brazilian, american, or modded like elysian - these must not generate neocultures
	std::set<std::string>
		 eu4OverriddenNeoCultureGroups; // Stuff like norse_g for vinland, spartakian_group etc, mostly Third Odyssey  - these must not generate neocultures
	std::map<std::string, std::string> eu4CultureToGroup; // Simple map castillian->iberian for eu4 cultures.

	ColonialRegionMapper colonialRegionMapper;
	WesternizationMapper westernizationMapper;
	NewEU4CultureMapper newEU4CultureMapper;
};
} // namespace mappers

#endif // CULTURE_MAPPER_H