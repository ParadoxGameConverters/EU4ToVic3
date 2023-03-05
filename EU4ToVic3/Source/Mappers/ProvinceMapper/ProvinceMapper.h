#ifndef PROVINCE_MAPPER_H
#define PROVINCE_MAPPER_H
#include "Parser.h"
#include "ProvinceMappingsVersion.h"
#include <map>

namespace mappers
{
class ProvinceMapper: commonItems::parser
{
  public:
	ProvinceMapper() = default;
	void loadProvinceMappings(const std::string& fileName);

	[[nodiscard]] std::vector<int> getEU4Provinces(const std::string& v3Province) const;
	[[nodiscard]] std::vector<std::string> getV3Provinces(int eu4Province) const;
	[[nodiscard]] const auto& getMappings() const { return mappings; }

  private:
	void registerKeys();
	void createMappings();

	std::map<std::string, std::vector<int>> V3ToEU4ProvinceMap;
	std::map<int, std::vector<std::string>> EU4ToVic3ProvinceMap;
	std::vector<ProvinceMapping> mappings;
};
} // namespace mappers

#endif // PROVINCE_MAPPER_H