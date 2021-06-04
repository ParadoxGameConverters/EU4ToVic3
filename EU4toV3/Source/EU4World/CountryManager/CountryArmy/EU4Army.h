#ifndef EU4_ARMY_H
#define EU4_ARMY_H
#include "EU4Regiment.h"
#include <vector>

namespace EU4
{
class UnitTypeLoader;
class EU4Army: public commonItems::convenientParser
{
  public:
	EU4Army() = default;
	EU4Army(std::istream& theStream, const std::string& potentialNavy); // Also applies to ships

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] auto getLocation() const { return location; }
	[[nodiscard]] auto getArmyFloats() const { return armyFloats; }
	[[nodiscard]] const auto& getRegiments() const { return regimentList; }

	[[nodiscard]] int getTotalTypeStrength(const std::string& type) const;

	void updateRegimentTypes(const UnitTypeLoader& unitTypeLoader);

  private:
	void registerKeys();

	std::string name;
	int location = 0; // provinceID of current army's location.
	bool armyFloats = false;
	std::vector<EU4Regiment> regimentList;
};
} // namespace EU4
#endif // EU4_ARMY_H
