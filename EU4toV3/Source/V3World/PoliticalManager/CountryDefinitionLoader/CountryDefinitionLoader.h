#ifndef COUNTRY_DEFINITION_LOADER_H
#define COUNTRY_DEFINITION_LOADER_H
#include "Country.h"
#include "Parser.h"
#include <map>
#include <string>

namespace V3
{
class CountryDefinitionLoader: commonItems::parser
{
  public:
	CountryDefinitionLoader() = default;

	void loadCommonCountries(const std::string& v3Path);
	[[nodiscard]] const auto& getCountries() const { return countries; }

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<Country>> countries;
};
} // namespace V3
#endif // COUNTRY_DEFINITION_LOADER_H