#ifndef VANILLA_POPULATION_HISTORY_LOADER_H
#define VANILLA_POPULATION_HISTORY_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class VanillaPopulationHistoryLoader: commonItems::parser
{
  public:
	VanillaPopulationHistoryLoader() = default;
	void loadVanillaPopulationHistories(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getPopulationHistoryElements() const { return populationHistoryElements; }

  private:
	void registerKeys();

	std::map<std::string, std::vector<std::string>> populationHistoryElements;
};
} // namespace V3
#endif // VANILLA_POPULATION_HISTORY_LOADER_H