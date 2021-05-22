#ifndef RELIGION_MAPPER_H
#define RELIGION_MAPPER_H

#include "Parser.h"
#include <map>
#include <optional>
#include <string>

namespace mappers
{
class ReligionMapper: commonItems::parser
{
  public:
	ReligionMapper();
	explicit ReligionMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getVic2Religion(const std::string& eu4Religion) const;
	void scrapeCustomReligions();

  private:
	void registerKeys();

	std::map<std::string, std::string> eu4ToVic2ReligionMap;
};
} // namespace mappers

#endif // RELIGION_MAPPER_H
