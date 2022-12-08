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
	[[nodiscard]] const auto& getUnMappedCultures() const { return unmappedCultures; }
	[[nodiscard]] const auto& getV3CultureDefinitions() const { return v3CultureDefinitions; }

	[[nodiscard]] std::optional<std::string> cultureMatch(const V3::ClayManager& clayManager,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::ReligionLoader& religionLoader,
		 const std::string& eu4culture,
		 const std::string& eu4religion,
		 const std::string& v3state,
		 const std::string& v3ownerTag,
		 bool neoCultureRequest = false);

	void generateCultureDefinitions(const commonItems::ModFilesystem& modFS,
		 const std::string& nameListsPath,
		 const std::string& nameListMapPath,
		 const std::string& cultureTraitsPath,
		 const EU4::CultureLoader& cultureLoader,
		 const EU4::EU4LocalizationLoader& eu4Locs);

	void injectReligionsIntoCultureDefs(const V3::ClayManager& clayManager);

  private:
	std::optional<std::string> getNeoCultureMatch(const std::string& eu4culture, const std::string& v3state, const V3::ClayManager& clayManager);
	void registerKeys();

	std::vector<CultureMappingRule> cultureMapRules;
	std::map<std::string, std::string> encounteredMacros;
	std::set<std::string> unmappedCultures; // same name for eu4 as for vic3.
	std::map<std::string, CultureDef> v3CultureDefinitions;
	std::map<std::string, std::map<std::string, std::string>> colonyNeoCultureTargets; // colony->[eu4 culture -> v3 neoculture]
};
} // namespace mappers

#endif // CULTURE_MAPPER_H