#ifndef EU4_PROVINCE_H
#define EU4_PROVINCE_H
#include "ConvenientParser.h"
#include "ProvinceBuildings.h"
#include "ProvinceHistory.h"

namespace EU4
{
class Province: commonItems::convenientParser
{
  public:
	Province() = default;
	Province(const std::string& numString, std::istream& theStream);

	[[nodiscard]] auto getID() const { return provID; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getOwnerTag() const { return ownerTag; }
	[[nodiscard]] const auto& getControllerTag() const { return controllerTag; }
	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }

	[[nodiscard]] auto inHre() const { return inHRE; }
	[[nodiscard]] auto isTerritorialCore() const { return territorialCore; }

	[[nodiscard]] auto getBaseTax() const { return baseTax; }
	[[nodiscard]] auto getBaseProduction() const { return baseProduction; }
	[[nodiscard]] auto getBaseManpower() const { return baseManpower; }
	[[nodiscard]] auto getStartingTax() const { return startingTax; }
	[[nodiscard]] auto getStartingProduction() const { return startingProduction; }
	[[nodiscard]] auto getStartingManpower() const { return startingManpower; }

	[[nodiscard]] const auto& getCores() const { return cores; }
	[[nodiscard]] const auto& getBuildings() const { return buildings; }
	[[nodiscard]] const auto& getProvinceHistory() const { return provinceHistory; }

	[[nodiscard]] auto hasBuilding(const std::string& building) const { return buildings.contains(building); }
	[[nodiscard]] auto hasGreatProject(const std::string& greatProject) const { return greatProjects.contains(greatProject); }

  private:
	void registerKeys();

	int provID = 0;
	std::string name;
	std::string ownerTag;
	std::string controllerTag;
	std::string culture;
	std::string religion;

	bool inHRE = false;
	bool territorialCore = false;

	double baseTax = 0;
	double baseProduction = 0;
	double baseManpower = 0;
	double startingTax = 0;
	double startingProduction = 0;
	double startingManpower = 0;

	ProvinceHistory provinceHistory;
	std::set<std::string> cores;
	std::set<std::string> buildings;
	std::set<std::string> greatProjects;
};
} // namespace EU4

#endif // EU4_PROVINCE_H
