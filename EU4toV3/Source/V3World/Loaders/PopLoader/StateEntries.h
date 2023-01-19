#ifndef STATE_ENTRIES_H
#define STATE_ENTRIES_H
#include "Parser.h"
#include "PopManager/Pops/StatePops.h"

namespace V3
{
class StateEntries: commonItems::parser
{
  public:
	StateEntries() = default;

	void loadStates(std::istream& theStream);
	[[nodiscard]] const auto& getStatePops() const { return statePops; }

  private:
	void registerKeys();
	[[nodiscard]] static std::optional<std::string> getStateFromName(const std::string& name);

	std::map<std::string, StatePops> statePops; // state, StatePops
};
} // namespace V3
#endif // STATE_ENTRIES_H