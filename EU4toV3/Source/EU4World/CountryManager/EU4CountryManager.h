#ifndef EU4_COUNTRY_MANAGER
#define EU4_COUNTRY_MANAGER
#include "EU4Country.h"
#include "Parser.h"
#include <map>
#include <memory>

namespace EU4
{
class CountryManager: public commonItems::parser
{
  public:
	CountryManager() = default;
	CountryManager(std::istream& theStream);

	[[nodiscard]] const auto& getCountries() const { return countries; }
	[[nodiscard]] std::shared_ptr<Country> getCountry(const std::string& tag) const;

  private:
	void registerKeys();
	
	std::map<std::string, std::shared_ptr<Country>> countries;
};
} // namespace EU4

#endif // EU4_COUNTRY_MANAGER