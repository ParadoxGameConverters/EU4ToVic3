#ifndef COLONIAL_REGION_MAPPER_H
#define COLONIAL_REGION_MAPPER_H
#include "ColonialRegionMapping.h"
#include "Parser.h"

namespace V3
{
class ClayManager;
}
namespace mappers
{
class ColonialRegionMapper: commonItems::parser
{
  public:
	ColonialRegionMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] const auto& getColonialRegions() const { return colonialRegions; }
	[[nodiscard]] std::optional<ColonialRegionMapping> getColonyForState(const std::string& v3State, const V3::ClayManager& clayManager) const;
	[[nodiscard]] std::optional<std::string> getColonyNameForState(const std::string& v3State, const V3::ClayManager& clayManager) const;

  private:
	void registerKeys();

	std::map<std::string, ColonialRegionMapping> colonialRegions;
};
} // namespace mappers

#endif // COUNTRY_MAPPINGS_H