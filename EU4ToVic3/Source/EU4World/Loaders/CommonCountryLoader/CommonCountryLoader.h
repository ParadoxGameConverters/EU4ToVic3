#ifndef COMMON_COUNTRY_LOADER
#define COMMON_COUNTRY_LOADER
#include "Color.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace EU4
{
class CommonCountryLoader: commonItems::parser
{
  public:
	CommonCountryLoader() = default;

	void loadCommonCountries(const commonItems::ModFilesystem& modFS);

	[[nodiscard]] std::optional<commonItems::Color> getCommonColor(const std::string& tag) const;

  private:
	void registerKeys();

	parser tagParser;

	std::map<std::string, commonItems::Color> countryColors;
	std::map<std::string, std::string> tagPaths;
	std::optional<commonItems::Color> color;
};
} // namespace EU4

#endif // COMMON_COUNTRY_LOADER