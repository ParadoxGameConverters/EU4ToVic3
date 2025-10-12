#ifndef SLAVE_CULTURE_MAPPER_H
#define SLAVE_CULTURE_MAPPER_H
#include "Parser.h"
#include <filesystem>

namespace mappers
{
class SlaveCultureMapper: commonItems::parser
{
  public:
	SlaveCultureMapper() = default;
	void loadMappingRules(const std::filesystem::path& filePath);

	[[nodiscard]] std::optional<std::string> getSlaveCulture(const std::string& masterTrait) const;

  private:
	void registerKeys();

	std::map<std::string, std::string> masterHeritageToSlaveCultureMap;
	std::map<std::string, std::string> masterLanguageToSlaveCultureMap;
};
} // namespace mappers

#endif // SLAVE_CULTURE_MAPPER_H
