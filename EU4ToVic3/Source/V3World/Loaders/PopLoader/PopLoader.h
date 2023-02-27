#ifndef POP_LOADER_H
#define POP_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "PopManager/Pops/StatePops.h"

namespace V3
{
class PopLoader: commonItems::parser
{
  public:
	PopLoader() = default;

	void loadPops(const commonItems::ModFilesystem& modFS);
	[[nodiscard]] const auto& getStatePops() const { return statePops; }

  private:
	void registerKeys();

	std::map<std::string, StatePops> statePops;
};
} // namespace V3
#endif // POP_LOADER_H