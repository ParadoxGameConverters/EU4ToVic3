#ifndef CULTURE_MAPPER_H
#define CULTURE_MAPPER_H
#include "CultureDefinitionLoader/CultureDef.h"
#include "CultureMappingRule.h"
#include "Parser.h"

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
	void expandCulturalMappings(const V3::ClayManager& clayManager, const EU4::CultureLoader& cultureLoader, const EU4::ReligionLoader& religionLoader);

	[[nodiscard]] const auto& getMacros() const { return encounteredMacros; }
	[[nodiscard]] const auto& getV3CultureDefinitions() const { return v3CultureDefinitions; }

	[[nodiscard]] std::optional<std::string> cultureMatch(const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 const std::string& v3state,
		 const std::string& v3ownerTag,
		 bool silent = false) const;
	[[nodiscard]] std::optional<std::string> cultureRegionalMatch(const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 const std::string& v3state,
		 const std::string& v3ownerTag) const;
	[[nodiscard]] std::optional<std::string> cultureNonRegionalNonReligiousMatch(const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 const std::string& v3state,
		 const std::string& v3ownerTag) const;

	void generateCultureDefinitions(const commonItems::ModFilesystem& modFS,
		 const std::string& nameListsPath,
		 const std::string& nameListMapPath,
		 const std::string& cultureTraitsPath,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::EU4LocalizationLoader& eu4Locs);

	void injectReligionsIntoCultureDefs(const V3::ClayManager& clayManager);

  private:
	void registerKeys();

	[[nodiscard]] static CultureDef generateCultureDefinition(const std::string& eu4CultureName,
		 const CultureTraitMapper& cultureTraitMapper,
		 const NameListMapper& nameListMapper,
		 const NameListLoader& nameListLoader,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::EU4LocalizationLoader& eu4Locs);
	[[nodiscard]] static std::set<std::string> breakDownCulturalName(const std::string& eu4CultureName);
	static void copyEU4Names(CultureDef& cultureDef, const EU4::CultureParser& sourceCulture);
	static void copyNamePoolNames(CultureDef& cultureDef, const NameListEntry& namePool);

	std::vector<CultureMappingRule> cultureMapRules;
	std::map<std::string, std::string> encounteredMacros;
	std::set<std::string> unmappedCultures; // same name for eu4 as for vic3.
	std::map<std::string, CultureDef> v3CultureDefinitions;
};
} // namespace mappers

#endif // CULTURE_MAPPER_H