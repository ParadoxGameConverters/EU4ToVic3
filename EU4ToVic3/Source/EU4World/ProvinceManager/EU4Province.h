#ifndef EU4_PROVINCE_H
#define EU4_PROVINCE_H
#include "BuildingCostLoader/BuildingCostLoader.h"
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
	void setOwnerTag(const std::string& theTag) { ownerTag = theTag; }
	[[nodiscard]] const auto& getControllerTag() const { return controllerTag; }
	void setControllerTag(const std::string& theTag) { controllerTag = theTag; }
	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getReligion() const { return religion; }
	void addCapital(const std::string& theTag) { capitals.emplace(theTag); }
	[[nodiscard]] const auto& getCapitals() const { return capitals; }

	[[nodiscard]] auto inHre() const { return inHRE; }
	[[nodiscard]] auto isTradeCompany() const { return tradeCompany; }
	[[nodiscard]] auto isTerritorialCore() const { return territorialCore; }

	[[nodiscard]] auto getBaseTax() const { return baseTax; }
	[[nodiscard]] auto getBaseProduction() const { return baseProduction; }
	[[nodiscard]] auto getBaseManpower() const { return baseManpower; }

	[[nodiscard]] const auto& getCores() const { return cores; }
	void addCore(const std::string& tag) { cores.insert(tag); }
	void removeCore(const std::string& tag) { cores.erase(tag); }

	[[nodiscard]] const auto& getBuildings() const { return buildings; }
	[[nodiscard]] const auto& getProvinceHistory() const { return provinceHistory; }
	void setProvinceHistory(const ProvinceHistory& history) { provinceHistory = history; }

	[[nodiscard]] auto hasBuilding(const std::string& building) const { return buildings.contains(building); }
	[[nodiscard]] auto hasGreatProject(const std::string& greatProject) const { return greatProjects.contains(greatProject); }

	[[nodiscard]] auto isSea() const { return sea; }
	void setSea() { sea = true; }

	void setAssimilationFactor(double factor) { assimilationFactor = factor; }
	void buildPopRatios(const DatingData& datingData) { provinceHistory.buildPopRatios(assimilationFactor, datingData); }
	void markNeoCulture(const std::string& theCulture) { provinceHistory.markNeoCulture(theCulture); }
	void purgeHistories() { provinceHistory.purgeHistories(); }

	[[nodiscard]] double getCulturePercent(const std::string& theCulture) const;
	[[nodiscard]] const auto& getStartingCulture() const { return provinceHistory.getStartingCulture(); }

	// These relate to province weight, to be used in pop shaping.
	void determineProvinceWeight(const BuildingCostLoader& buildingTypes);
	[[nodiscard]] auto getProvinceWeight() const { return provinceWeight; }
	[[nodiscard]] auto getInvestedWeight() const { return investedWeight; }
	void setInvestmentFactor(double factor) { investmentFactor = factor; }
	[[nodiscard]] auto getInvestmentFactor() const { return investmentFactor; }

  private:
	void registerKeys();

	int provID = 0;
	std::string name;
	std::string ownerTag;
	std::string controllerTag;
	std::string culture;
	std::string religion;
	std::set<std::string> capitals; // eu4 tags for whichever countries this is capital.

	bool inHRE = false;
	bool territorialCore = false;
	bool tradeCompany = false;
	bool sea = false;

	// province attributes for weights
	double baseTax = 0;
	double baseProduction = 0;
	double baseManpower = 0;
	double provinceWeight = 0;	  // absolute dev + buildings
	double investedWeight = 0;	  // devpushed dev + buildings
	double investmentFactor = 0; // a log function to normalise the investedWeight.

	ProvinceHistory provinceHistory;
	std::set<std::string> cores;
	std::set<std::string> buildings;
	std::set<std::string> greatProjects;

	double assimilationFactor = 0;
};
} // namespace EU4

#endif // EU4_PROVINCE_H
