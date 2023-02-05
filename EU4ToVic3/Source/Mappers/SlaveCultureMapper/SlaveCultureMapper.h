#ifndef SLAVE_CULTURE_MAPPER_H
#define SLAVE_CULTURE_MAPPER_H
#include "Parser.h"

namespace mappers
{
class SlaveCultureMapper: commonItems::parser
{
  public:
	SlaveCultureMapper() = default;
	void loadMappingRules(const std::string& filePath);

	[[nodiscard]] std::optional<std::string> getSlaveCulture(const std::string& masterTrait) const;

  private:
	void registerKeys();

	std::map<std::string, std::string> masterTraitToSlaveCultureMap;
};
} // namespace mappers

#endif // SLAVE_CULTURE_MAPPER_H
