#ifndef COLONIAL_TAG_MAPPER_H
#define COLONIAL_TAG_MAPPER_H
#include "ColonialTagMapping.h"
#include "Parser.h"

namespace V3
{
class ClayManager;
}
namespace mappers
{
class ColonialTagMapper: commonItems::parser
{
  public:
	ColonialTagMapper() = default;
	void loadMappingRules(const std::string& filePath);

	[[nodiscard]] std::optional<std::string> matchColonialTag(const V3::Country& country,
		 const ColonialRegionMapper& colonialRegionMapper,
		 const V3::ClayManager& clayManager) const;

  private:
	void registerKeys();

	std::vector<ColonialTagMapping> colonialTagMappings;
	std::set<std::string> knownColonialTags;
};
} // namespace mappers

#endif // COLONIAL_TAG_MAPPER_H