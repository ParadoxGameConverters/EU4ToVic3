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

	[[nodiscard]] auto getNum() const { return num; }
	[[nodiscard]] auto inHre() const { return inHRE; }
	[[nodiscard]] auto isTerritorialCore() const { return territorialCore; }
	[[nodiscard]] auto isColony() const { return colony; }
	[[nodiscard]] auto isCity() const { return city; }
	[[nodiscard]] auto wasColonized() const { return hadOriginalColonizer || provinceHistory.wasColonized(); }

	[[nodiscard]] auto getBaseTax() const { return baseTax; }
	[[nodiscard]] auto getBaseProduction() const { return baseProduction; }
	[[nodiscard]] auto getBaseManpower() const { return baseManpower; }
	[[nodiscard]] auto getFirstOwnedDate() const { return provinceHistory.getFirstOwnedDate(); }
	[[nodiscard]] auto hasBuilding(const std::string& building) const { return buildings.contains(building); }
	[[nodiscard]] auto hasGreatProject(const std::string& greatProject) const { return greatProjects.count(greatProject); }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getOwnerString() const { return ownerString; }
	[[nodiscard]] const auto& getControllerString() const { return controllerString; }
	[[nodiscard]] const auto& getOriginalCulture() const { return provinceHistory.getStartingCulture(); }
	[[nodiscard]] const auto& getCores() const { return cores; }
	[[nodiscard]] const auto& getBuildings() const { return buildings; }

  private:
	void registerKeys();

	int num = 0;
	std::string name;
	std::string ownerString;
	std::string controllerString;
	std::string culture;
	std::string religion;

	std::set<std::string> cores;

	bool inHRE = false;
	bool colony = false;
	bool hadOriginalColonizer = false;
	bool territorialCore = false;
	bool city = false;

	ProvinceHistory provinceHistory;
	std::set<std::string> buildings;
	std::set<std::string> greatProjects;

	// province attributes for weights
	double baseTax = 0;
	double baseProduction = 0;
	double baseManpower = 0;
	double provinceWeight = 0;
	double investmentFactor = 0;
};
} // namespace EU4

#endif // EU4_PROVINCE_H
