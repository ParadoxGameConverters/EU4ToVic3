#ifndef RELIGION_MAPPER_H
#define RELIGION_MAPPER_H
#include "Parser.h"
#include "ReligionLoader/ReligionLoader.h"
#include <map>
#include <optional>
#include <string>

namespace mappers
{
class ReligionMapper: commonItems::parser
{
  public:
	ReligionMapper() = default;
	void loadMappingRules(const std::string& filePath);
	void loadMappingRules(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getV3Religion(const std::string& eu4Religion) const;
	void expandReligionMappings(const std::map<std::string, EU4::Religion>& religions);

  private:
	void registerKeys();

	std::map<std::string, std::string> eu4ToV3ReligionMap;
};
} // namespace mappers

#endif // RELIGION_MAPPER_H
