#ifndef IG_IDEOLOGIES_MAPPER_H
#define IG_IDEOLOGIES_MAPPER_H
#include "IGIdeologiesMapping.h"
#include "Parser.h"

namespace mappers
{
class IGIdeologiesMapper: commonItems::parser
{
  public:
	IGIdeologiesMapper() = default;
	void loadMappingRules(const std::string& filePath);

	[[nodiscard]] std::map<std::string, IGIdeologyMod> getIGIdeologyMods(const V3::Country& country,
		 const CultureMapper& cultureMapper,
		 const ReligionMapper& religionMapper,
		 const V3::ClayManager& clayManager) const;

  private:
	void registerKeys();

	std::vector<IGIdeologiesMapping> mappings;
};
} // namespace mappers

#endif // IG_IDEOLOGIES_MAPPER_H