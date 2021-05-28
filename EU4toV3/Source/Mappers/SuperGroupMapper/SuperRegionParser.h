#ifndef SUPERREGION_PARSER
#define SUPERREGION_PARSER
#include "Parser.h"
#include <set>

namespace mappers
{
class SuperRegionParser: commonItems::parser
{
  public:
	explicit SuperRegionParser(std::istream& theStream);

	[[nodiscard]] const auto& getSuperRegionAssimilationMap() const { return superRegionAssimilationMap; }
	[[nodiscard]] const auto& getSuperRegionNames() const { return superRegionNames; }

  private:
	void registerKeys();

	std::map<std::string, int> superRegionAssimilationMap;
	std::set<std::string> superRegionNames;
};
} // namespace mappers

#endif // SUPERREGION_PARSER