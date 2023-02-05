#ifndef VANILLA_STATE_ENTRIES_H
#define VANILLA_STATE_ENTRIES_H
#include "Parser.h"
#include "VanillaStateEntry.h"

namespace V3
{
class VanillaStateEntries: commonItems::parser
{
  public:
	VanillaStateEntries() = default;
	explicit VanillaStateEntries(std::istream& theStream);

	[[nodiscard]] const auto& getStates() const { return stateEntries; }

  private:
	void registerKeys();

	std::map<std::string, VanillaStateEntry> stateEntries;
};
} // namespace V3
#endif // VANILLA_STATE_ENTRIES_H