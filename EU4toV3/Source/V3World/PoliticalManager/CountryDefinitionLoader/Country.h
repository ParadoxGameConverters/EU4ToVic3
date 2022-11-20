#ifndef V3_COUNTRY_H
#define V3_COUNTRY_H
#include "ClayManager/SubState.h"
#include "Parser.h"
#include <CountryManager/EU4Country.h>
#include <memory>
#include <string>

namespace V3
{
struct VanillaCommonCountryData
{
	std::string type;
	std::string tier;
	std::set<std::string> cultures;
	std::string religion;
	std::string capitalStateName;
	std::optional<commonItems::Color> color;
	bool is_named_from_capital = false;
};

class Country: commonItems::convenientParser
{
  public:
	Country() = default;
	void initializeCountry(std::istream& theStream);
	void setTag(const std::string& theTag) { tag = theTag; }

	[[nodiscard]] const auto& getTag() const { return tag; }
	[[nodiscard]] const auto& getVanillaData() const { return vanillaData; }
	[[nodiscard]] const auto& getSourceCountry() const { return sourceCountry; }
	[[nodiscard]] const auto& getSubStates() const { return substates; }


  private:
	void registerKeys();

	std::string tag;
	std::optional<VanillaCommonCountryData> vanillaData;

	std::shared_ptr<EU4::Country> sourceCountry;
	std::vector<std::shared_ptr<SubState>> substates;
};
} // namespace V3

#endif // V3_COUNTRY_H
