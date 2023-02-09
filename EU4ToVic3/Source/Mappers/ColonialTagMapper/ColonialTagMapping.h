#ifndef COLONIAL_TAG_MAPPING_H
#define COLONIAL_TAG_MAPPING_H
#include "Parser.h"

namespace V3
{
class Country;
class ClayManager;
} // namespace V3
namespace mappers
{
class ColonialRegionMapper;
class ColonialTagMapping: commonItems::parser
{
  public:
	ColonialTagMapping() = default;
	explicit ColonialTagMapping(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> matchColonialTag(const V3::Country& country,
		 const ColonialRegionMapper& colonialRegionMapper,
		 const V3::ClayManager& clayManager) const;
	[[nodiscard]] const auto& getTag() const { return tag; }

  private:
	void registerKeys();

	std::string region;
	std::string aloneName;
	std::string tag;
};
} // namespace mappers

#endif // COLONIAL_TAG_MAPPING_H