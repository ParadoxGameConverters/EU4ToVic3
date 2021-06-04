#ifndef EU4_REGIMENT
#define EU4_REGIMENT
#include "ConvenientParser.h"
#include "UnitTypeLoader/UnitType.h"

namespace EU4
{
class EU4Regiment: public commonItems::convenientParser
{
  public:
	EU4Regiment() = default;
	explicit EU4Regiment(std::istream& theStream); // Also applies to ships

	[[nodiscard]] auto getHome() const { return home; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getType() const { return unitType.unitType; }
	[[nodiscard]] auto getStrength() const { return unitType.strength; }

	void updateUnitType(const UnitType& theType) { unitType = theType; }

  private:
	void registerKeys();

	int home = 0; // provinceID of regiment conscription, ie. home.
	std::string name;
	UnitType unitType; // We load it as "western_medieval_knights" but later update to "infantry", with set strength.
};
} // namespace EU4

#endif // EU4_REGIMENT