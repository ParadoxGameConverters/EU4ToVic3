#ifndef VANILLA_COUNTRY_HISTORY_LOADER_H
#define VANILLA_COUNTRY_HISTORY_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace V3
{
class VanillaCountryHistoryLoader: commonItems::parser
{
  public:
	VanillaCountryHistoryLoader() = default;
	void loadVanillaCountryHistories(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] const auto& getCountryHistoryElements() const { return countryHistoryElements; }

  private:
	void registerKeys();

	std::map<std::string, std::vector<std::string>> countryHistoryElements;
};
} // namespace V3
#endif // VANILLA_COUNTRY_HISTORY_LOADER_H