#ifndef COUNTRY_DEFINITION_LOADER_H
#define COUNTRY_DEFINITION_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include <map>
#include <string>

namespace V3
{
class Country;
class CountryDefinitionLoader: commonItems::parser
{
  public:
	CountryDefinitionLoader() = default;

	void loadCommonCountries(const commonItems::ModFilesystem& modFS);
	void reloadCommonCountries(const commonItems::ModFilesystem& vanillaFS);
	[[nodiscard]] const auto& getCountries() const { return countries; }

  private:
	void registerKeys();
	void registerReloadKeys();

	std::map<std::string, std::shared_ptr<Country>> countries;
};
} // namespace V3
#endif // COUNTRY_DEFINITION_LOADER_H