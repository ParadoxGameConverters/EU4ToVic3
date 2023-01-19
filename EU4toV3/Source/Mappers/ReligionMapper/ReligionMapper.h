#ifndef RELIGION_MAPPER_H
#define RELIGION_MAPPER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "ReligionMapper/ReligionDefinitionLoader/ReligionDef.h"
#include <map>
#include <optional>
#include <string>

namespace EU4
{
struct Religion;
class EU4LocalizationLoader;
} // namespace EU4
namespace mappers
{
class ReligionGroupMapper;
class ReligionDefinitionLoader;
class ReligionMapper: commonItems::parser
{
  public:
	ReligionMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getV3Religion(const std::string& eu4Religion) const;
	[[nodiscard]] const auto& getV3ReligionDefinitions() const { return vic3ReligionDefinitions; }

	void expandReligionMappings(const std::map<std::string, EU4::Religion>& eu4Religions);
	void generateReligionDefinitions(const commonItems::ModFilesystem& modFS,
		 const std::string& groupMapPath,
		 const std::map<std::string, EU4::Religion>& eu4Religions,
		 const EU4::EU4LocalizationLoader& eu4Locs);

  private:
	void registerKeys();

	[[nodiscard]] ReligionDef generateReligionDefinition(const std::string& v3ReligionName,
		 const ReligionGroupMapper& religionGroupMapper,
		 const ReligionDefinitionLoader& religionDefinitionLoader,
		 const EU4::Religion& eu4Religion,
		 const EU4::EU4LocalizationLoader& eu4Locs) const;

	std::map<std::string, std::string> eu4ToV3ReligionMap;
	std::map<std::string, ReligionDef> vic3ReligionDefinitions;
};
} // namespace mappers

#endif // RELIGION_MAPPER_H
