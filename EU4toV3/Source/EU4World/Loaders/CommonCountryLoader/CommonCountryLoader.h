#ifndef COMMON_COUNTRY_LOADER
#define COMMON_COUNTRY_LOADER
#include "Color.h"
#include "ModLoader/ModLoader.h"
#include "Parser.h"

namespace EU4
{
class CommonCountryLoader: commonItems::parser
{
  public:
	CommonCountryLoader() = default;

	void loadCommonCountries(const std::string& EU4Path, const Mods& mods);

	[[nodiscard]] std::optional<commonItems::Color> getCommonColor(const std::string& tag) const;

  private:
	void readCommonCountriesFile(std::istream& in, const std::string& rootPath);

	void registerKeys();

	std::map<std::string, commonItems::Color> countryColors;
	std::map<std::string, std::string> tagPaths;
	std::optional<commonItems::Color> color;
};
} // namespace EU4

#endif // COMMON_COUNTRY_LOADER