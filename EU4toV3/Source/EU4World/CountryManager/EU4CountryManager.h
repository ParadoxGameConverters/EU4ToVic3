#ifndef EU4_COUNTRY_MANAGER
#define EU4_COUNTRY_MANAGER
#include "EU4Country.h"
#include "Parser.h"
#include <map>
#include <memory>
#include "UnitTypeLoader/UnitTypeLoader.h"

namespace EU4
{
class CountryManager: public commonItems::parser
{
  public:
	CountryManager() = default;
	
	void loadCountries(std::istream& theStream);

	[[nodiscard]] const auto& getCountries() const { return countries; }
	[[nodiscard]] std::shared_ptr<Country> getCountry(const std::string& tag) const;

	void loadUnitTypes(const std::string& EU4Path, const Mods& mods);
	void updateUnitTypes();

  private:
	void registerKeys();
	
	std::map<std::string, std::shared_ptr<Country>> countries;

	UnitTypeLoader unitTypeLoader;
};
} // namespace EU4

#endif // EU4_COUNTRY_MANAGER