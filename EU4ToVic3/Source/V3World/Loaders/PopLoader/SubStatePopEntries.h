#ifndef SUBSTATE_POP_ENTRIES_H
#define SUBSTATE_POP_ENTRIES_H
#include "Parser.h"
#include "PopManager/Pops/Pop.h"

namespace V3
{
class SubStatePopEntries: commonItems::parser
{
  public:
	SubStatePopEntries() = default;

	void loadPops(std::istream& theStream);
	[[nodiscard]] const auto& getPops() const { return pops; }

  private:
	void registerKeys();

	std::vector<Pop> pops;
};
} // namespace V3
#endif // SUBSTATE_POP_ENTRIES_H