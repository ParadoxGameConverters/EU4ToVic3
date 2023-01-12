#ifndef VANILLA_BUILDING_ENTRIES_H
#define VANILLA_BUILDING_ENTRIES_H
#include "Parser.h"

namespace V3
{
class VanillaBuildingEntries: commonItems::parser
{
  public:
	VanillaBuildingEntries() = default;
	explicit VanillaBuildingEntries(std::istream& theStream);

	[[nodiscard]] const auto& getBuildingElements() const { return buildingElements; }

  private:
	void registerKeys();

	parser entryParser;
	std::string activeTag;

	std::map<std::string, std::vector<std::string>> buildingElements; // unparsed elements from the file, ready for direct dump.
};
} // namespace V3
#endif // VANILLA_BUILDING_ENTRIES_H