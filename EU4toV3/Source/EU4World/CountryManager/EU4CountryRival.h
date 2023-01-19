#ifndef EU4_COUNTRY_RIVAL_H
#define EU4_COUNTRY_RIVAL_H
#include "ConvenientParser.h"

namespace EU4
{
class EU4CountryRival: commonItems::convenientParser
{
  public:
	EU4CountryRival() = default;
	explicit EU4CountryRival(std::istream& theStream);

	[[nodiscard]] const auto& getCountry() const { return country; }

  private:
	void registerKeys();

	std::string country;
};
} // namespace EU4

#endif // EU4_COUNTRY_RIVAL_H