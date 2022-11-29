#ifndef SUBSTATE_ENTRIES_H
#define SUBSTATE_ENTRIES_H
#include "Parser.h"
#include "PopManager/Pops/SubStatePops.h"

namespace V3
{
typedef std::map<std::string, std::vector<Pop>> ownedPops;

class SubStateEntries: commonItems::parser
{
  public:
	SubStateEntries() = default;

	void loadSubStates(std::istream& theStream);
	[[nodiscard]] const auto& getSubStatePops() const { return subStatePops; }

  private:
	void registerKeys();
	[[nodiscard]] static std::optional<std::string> getTagFromName(const std::string& name);

	std::vector<SubStatePops> subStatePops;
};
} // namespace V3
#endif // SUBSTATE_ENTRIES_H