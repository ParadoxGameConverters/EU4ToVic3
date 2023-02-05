#ifndef TECH_SETUP_MAPPING_H
#define TECH_SETUP_MAPPING_H
#include "Parser.h"

namespace mappers
{
class TechSetupMapping: commonItems::parser
{
  public:
	TechSetupMapping() = default;
	explicit TechSetupMapping(std::istream& theStream);

	[[nodiscard]] const auto& getTechScores() const { return techScores; }

  private:
	void registerKeys();

	std::map<std::string, int> techScores;
};
} // namespace mappers

#endif // TECH_SETUP_MAPPING_H