#ifndef PROVINCE_MAPPER_H
#define PROVINCE_MAPPER_H
#include "Parser.h"
#include "ProvinceMappingsVersion.h"
#include <set>
#include "ColonialRegions/ColonialRegions.h"

class Configuration;
namespace mappers
{
class ProvinceMapper: commonItems::parser
{
  public:
	ProvinceMapper();
	explicit ProvinceMapper(std::istream& mainStream, std::istream& colonialStream, const Configuration& testConfiguration);

	[[nodiscard]] std::set<int> getVic2ProvinceNumbers(int eu4ProvinceNumber) const;
	[[nodiscard]] std::set<int> getEU4ProvinceNumbers(int vic2ProvinceNumber) const;
	[[nodiscard]] std::optional<std::string> getColonialRegionForProvince(int province) const;

  private:
	void registerKeys();
	
	[[nodiscard]] ProvinceMappingsVersion getBestMappingsVersion(const GameVersion& usedEU4Version) const;
	void createMappings(const ProvinceMappingsVersion& provinceMappingsVersion);

	std::map<int, std::set<int>> vic2ToEU4ProvinceMap;
	std::map<int, std::set<int>> eu4ToVic2ProvinceMap;
	std::unique_ptr<EU4::ColonialRegions> colonialRegionsMapper;
	std::map<GameVersion, ProvinceMappingsVersion> mappingVersions;
};
} // namespace mappers

#endif // PROVINCE_MAPPER_H