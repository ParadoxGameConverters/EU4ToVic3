#ifndef PROVINCE_MAPPING_VERSION_H
#define PROVINCE_MAPPING_VERSION_H
#include "GameVersion.h"
#include "Parser.h"
#include "ProvinceMapping.h"

namespace mappers
{
class ProvinceMappingsVersion: commonItems::parser
{
  public:
	ProvinceMappingsVersion(const std::string& versionString, std::istream& theStream);

	[[nodiscard]] const auto& getVersion() const { return version; }
	[[nodiscard]] const auto& getMappings() const { return mappings; }

  private:
	void registerKeys();

	GameVersion version;
	std::vector<ProvinceMapping> mappings;
};
} // namespace mappers

#endif // PROVINCE_MAPPING_VERSION_H