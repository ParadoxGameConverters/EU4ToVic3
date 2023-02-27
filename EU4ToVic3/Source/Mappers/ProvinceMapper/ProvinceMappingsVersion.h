#ifndef PROVINCE_MAPPING_VERSION_H
#define PROVINCE_MAPPING_VERSION_H
#include "Parser.h"
#include "ProvinceMapping.h"

namespace mappers
{
class ProvinceMappingsVersion: commonItems::parser
{
  public:
	ProvinceMappingsVersion() = default;
	explicit ProvinceMappingsVersion(std::istream& theStream);

	[[nodiscard]] const auto& getMappings() { return mappings; }

  private:
	void registerKeys();

	std::vector<ProvinceMapping> mappings;
};
} // namespace mappers

#endif // PROVINCE_MAPPING_VERSION_H