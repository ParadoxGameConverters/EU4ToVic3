#ifndef VANILLA_STATE_LOADER_H
#define VANILLA_STATE_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "VanillaStateEntry.h"

namespace V3
{
class VanillaStateLoader: commonItems::parser
{
  public:
	VanillaStateLoader() = default;
	void loadVanillaStates(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getStates() const { return stateEntries; }

  private:
	void registerKeys();

	std::map<std::string, VanillaStateEntry> stateEntries;
};
} // namespace V3
#endif // VANILLA_STATE_LOADER_H