#ifndef EU4_PROVINCE_H
#define EU4_PROVINCE_H
#include "Buildings/Buildings.h"
#include "ConvenientParser.h"
#include "ProvinceBuildings.h"
#include "ProvinceHistory.h"
#include "Regions/Regions.h"
#include "SuperGroupMapper/SuperGroupMapper.h"

namespace EU4
{
class Province: commonItems::convenientParser
{
  public:
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
	[[nodiscard]] auto hasBuilding(const std::string& building) const { return buildings.hasBuilding(building); }
	[[nodiscard]] auto hasGreatProject(const std::string& greatProject) const { return greatProjects.count(greatProject); }

	// These relate to province weight, to be used in pop shaping.
	[[nodiscard]] auto getProvinceWeight() const { return provinceWeight; }
	[[nodiscard]] auto getInvestmentFactor() const { return investmentFactor; }

	[[nodiscard]] const auto& getArea() const { return areaName; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getOwnerString() const { return ownerString; }
	[[nodiscard]] const auto& getControllerString() const { return controllerString; }
	[[nodiscard]] const auto& getOriginalCulture() const { return provinceHistory.getStartingCulture(); }
	[[nodiscard]] const auto& getCores() const { return cores; }
	[[nodiscard]] const auto& getPopRatios() const { return provinceHistory.getPopRatios(); }
	[[nodiscard]] const auto& getBuildings() const { return buildings.getBuildings(); }

	[[nodiscard]] double getCulturePercent(const std::string& theCulture) const;

	void addCore(const std::string& tag) { cores.insert(tag); }
	void removeCore(const std::string& tag) { cores.erase(tag); }
	void setOwnerString(const std::string& _owner) { ownerString = _owner; }
	void setControllerString(const std::string& _controller) { controllerString = _controller; }
	void setArea(const std::string& theArea) { areaName = theArea; }
	void determineProvinceWeight(const mappers::Buildings& buildingTypes);
	void updatePopRatioCulture(const std::string& oldCultureName, const std::string& neoCultureName, const std::string& superRegion)
	{
		provinceHistory.updatePopRatioCulture(oldCultureName, neoCultureName, superRegion);
	}
	void buildPopRatio(const mappers::SuperGroupMapper& superGroupMapper, const Regions& regions);

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
	ProvinceBuildings buildings;

	std::set<std::string> greatProjects;
	std::string areaName;

	// province attributes for weights
	double baseTax = 0;
	double baseProduction = 0;
	double baseManpower = 0;
	double provinceWeight = 0;
	double investmentFactor = 0;
};
} // namespace EU4

#endif // EU4_PROVINCE_H
