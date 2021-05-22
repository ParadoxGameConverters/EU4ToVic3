#ifndef EU4_ARMY_H
#define EU4_ARMY_H
#include "EU4Regiment.h"
#include <vector>

namespace mappers
{
class UnitTypeMapper;
}
namespace EU4
{
class EU4Army: public commonItems::parser
{
  public:
	EU4Army() = default;
	EU4Army(std::istream& theStream, const std::string& potentialNavy); // Also applies to ships

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] auto getLocation() const { return location; }
	[[nodiscard]] auto getArmyFloats() const { return armyFloats; }
	[[nodiscard]] const auto& getRegiments() const { return regimentList; }

	[[nodiscard]] int getTotalTypeStrength(const std::string& category) const;

	void resolveRegimentTypes(const mappers::UnitTypeMapper& unitTypeMapper);

  private:
	void registerKeys();

	std::string name;
	int location = 0;
	bool armyFloats = false;
	std::vector<EU4Regiment> regimentList;
};
} // namespace EU4
#endif // EU4_ARMY_H
