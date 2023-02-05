#ifndef VANILLA_STATE_ENTRY_H
#define VANILLA_STATE_ENTRY_H
#include "Parser.h"
#include "VanillaSubStateEntry.h"

namespace V3
{
class VanillaStateEntry: commonItems::parser
{
  public:
	VanillaStateEntry() = default;
	explicit VanillaStateEntry(std::istream& theStream);

	[[nodiscard]] const auto& getHomelands() const { return homelands; }
	[[nodiscard]] const auto& getSubStates() const { return subStateEntries; }

  private:
	void registerKeys();

	std::set<std::string> homelands;
	std::vector<VanillaSubStateEntry> subStateEntries;
};
} // namespace V3
#endif // VANILLA_STATE_ENTRY_H