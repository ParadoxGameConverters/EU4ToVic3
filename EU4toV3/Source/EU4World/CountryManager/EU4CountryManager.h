#ifndef EU4_COUNTRY_MANAGER
#define EU4_COUNTRY_MANAGER
#include "CommonCountryLoader/CommonCountryLoader.h"
#include "EU4Country.h"
#include "Parser.h"
#include "ProvinceManager/ProvinceManager.h"
#include "UnitTypeLoader/UnitTypeLoader.h"
#include <map>
#include <memory>

namespace EU4
{
class CountryManager: public commonItems::parser
{
  public:
	CountryManager() = default;

	void loadCountries(std::istream& theStream);

	[[nodiscard]] const auto& getCountries() const { return countries; }
	[[nodiscard]] std::shared_ptr<Country> getCountry(const std::string& tag) const;

	// the prep
	void loadUnitTypes(const std::string& EU4Path, const Mods& mods);
	void loadCommonCountries(const std::string& EU4Path, const Mods& mods);

	// the weave
	void updateUnitTypes();
	void linkProvincesToCountries(const ProvinceManager& provinceManager);
	void setHREAndEmperors(const std::string& HREmperor, const std::string& celestialEmperor, const ProvinceManager& provinceManager);
	void setRevolutionTarget(const std::string& revolutionTarget);
	void buildManufactoryCounts() const;
	void injectColorsIntoCountries();

  private:
	void registerKeys();

	std::map<std::string, std::shared_ptr<Country>> countries;

	UnitTypeLoader unitTypeLoader;
	CommonCountryLoader commonCountryLoader;
};
} // namespace EU4

#endif // EU4_COUNTRY_MANAGER