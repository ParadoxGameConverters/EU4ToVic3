#ifndef COUNTRY_DEFINITION_LOADER_H
#define COUNTRY_DEFINITION_LOADER_H
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
	[[nodiscard]] const auto& getCountries() const { return countries; }

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<Country>> countries;
};
} // namespace V3
#endif // COUNTRY_DEFINITION_LOADER_H