#ifndef POP_ENTRY_H
#define POP_ENTRY_H
#include "Parser.h"
#include "PopManager/Pops/Pop.h"

namespace V3
{
class PopEntry: commonItems::parser
{
  public:
	PopEntry() = default;

	void loadPop(std::istream& theStream);
	[[nodiscard]] const auto& getPop() const { return pop; }

  private:
	void registerKeys();

	Pop pop;
};
} // namespace V3
#endif // POP_ENTRY_H