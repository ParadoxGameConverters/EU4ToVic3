#ifndef V3_COUNTRY_H
#define V3_COUNTRY_H
#include "ClayManager/SubState.h"
#include <CountryManager/EU4Country.h>
#include <memory>
#include <string>

namespace V3
{
class Country
{
  public:
	Country() = default;

  private:
	std::shared_ptr<EU4::Country> sourceCountry;
	std::string tag;

	std::vector<std::shared_ptr<SubState>> substates;
};
} // namespace V3

#endif // V3_COUNTRY_H
