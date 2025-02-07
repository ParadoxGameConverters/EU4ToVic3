#ifndef ECONOMY_MANAGER_H
#define ECONOMY_MANAGER_H
#include "Building/Building.h"
#include "Building/BuildingGroups.h"
#include "BuildingMapper/BuildingMapper.h"
#include "BuildingMapper/ProductionMethodMapper/ProductionMethodMapper.h"
#include "ClayManager/State/StateModifier.h"
#include "Configuration.h"
#include "CultureMapper/CultureDefinitionLoader/CultureDef.h"
#include "Demand/MarketTracker.h"
#include "EconomyManager/Building/ProductionMethods/ProductionMethod.h"
#include "EconomyManager/Building/ProductionMethods/ProductionMethodGroup.h"
#include "Loaders/BuildingLoader/OwnershipLoader/OwnershipLoader.h"
#include "Loaders/DefinesLoader/EconDefinesLoader.h"
#include "Loaders/DefinesLoader/Vic3DefinesLoader.h"
#include "Loaders/DemandLoader/DemandLoader.h"
#include "Loaders/NationalBudgetLoader/NationalBudgetLoader.h"
#include "Loaders/PopLoader/PopTypeLoader.h"
#include "Loaders/TechLoader/TechLoader.h"
#include "PoliticalManager/PoliticalManager.h"
#include "ReligionMapper/ReligionDefinitionLoader/ReligionDef.h"

namespace V3
{
class Country;
class Sector;
/*
 * PreReqs: Clay(Substates merged under the right country), Pops, Laws, Tech
 * all must be converted first.
 *
 *
 * Primarily handles buildings
 * 1. Load in centralized countries.
 * 2. Read in Mappers & Configs.
 * 3. Prepare country specific estimates. Ownership fractions, PMs used, PM fractions, etc.
 * 4. Bureaucracy! Have to handle it separate for game balance. Hand out generation that ~ matches need.
 * 4b. Ports! Easy for markets to end up cut off from your own market without giving each eligible state 1 port.
 * 5. For each centralized country get a CP budget based on fronter option.
 * 6. For each substate in a centralized country get a CP budget based on fronter option and terrain/state modifiers.
 * 7. Figure out the "national budget" of each country using the sector blueprints in NationalBudgetLoader.
 * 7b. NationalBudget is a list of sectors like 30% Farming, 25% Light Industry, etc.
 * 7c. Each Sector has a list of buildings that fall under it.
 * 8. Each Substate scores buildings based on market demand, EU4 buildings, terrain, state modifiers, and other factors.
 * 8b. Only buildings that are valid (resource/tech-wise) are scored.
 * 9. A negotiation between the states and their country about what to build.
 * 9b. The state w/ the most CP asks to build it's highest scoring building.
 * 9c. The country says, that building is in sector A and as a country we have X CP to spend in that sector.
 * 9d. The state then builds as many buildings of that kind as it can, limited by capacity, packet size and sector CP.
 * 9e. A bunch of small details that make this flow until all CP is assigned. Repeat for each country.
 *
 * 10. Now that buildings are built, we assign investors/owners using ownership sector blueprints
 * 11. Set Production Methods from a config file.
 *
 * This *should* give nice diverse, sensible, game-balanced economies while maintaining a reasonable configuration footprint.
 */
class EconomyManager
{
  public:
	EconomyManager() = default;
	void loadCentralizedStates(const std::map<std::string, std::shared_ptr<Country>>& countries);
	void loadMappersAndConfigs(const commonItems::ModFilesystem& modFS, const std::string& filePath = "");

	void assignCountryCPBudgets(Configuration::ECONOMY economyType,
		 Configuration::STARTDATE startDate,
		 const DatingData& dateData,
		 const PoliticalManager& politicalManager,
		 bool vn = false) const;
	void assignSubStateCPBudgets(Configuration::ECONOMY economyType) const;
	void balanceNationalBudgets() const;
	void buildBuildings(const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, mappers::CultureDef>& cultures,
		 const std::map<std::string, mappers::ReligionDef>& religions,
		 const Vic3DefinesLoader& defines) const;
	void investCapital(const std::map<std::string, std::shared_ptr<Country>>& countries) const;
	void setPMs() const;

	[[nodiscard]] const auto& getCentralizedCountries() const { return centralizedCountries; }

  private:
	static double calculatePopDistanceFactor(int countryPopulation, double geoMeanPopulation);
	static double calculateDateFactor(Configuration::STARTDATE startDate, const DatingData& dateData);
	static std::map<std::string, double> calcInvestorFractions(const std::map<std::string, OwnershipData>& buildingOwnershipMap, const Country& country);

	// Budget fxns set weight for all countries, accumulates the total weight, and returns a modifier to the globalCP pool (if any).
	[[nodiscard]] std::pair<double, double> countryBudgetCalcs(Configuration::ECONOMY economyType) const; // Return total weight, any special factors
	[[nodiscard]] std::pair<double, double> civLevelCountryBudgets() const;
	[[nodiscard]] std::pair<double, double> devCountryBudgets(Configuration::ECONOMY perCapitaType) const;
	[[nodiscard]] double getBaseSubStateWeight(const std::shared_ptr<SubState>& subState, Configuration::ECONOMY economyType) const;

	[[nodiscard]] double calculateGeoMeanCentralizedPops() const;
	[[nodiscard]] std::string pickBureaucracyPM(const Country& country) const;
	[[nodiscard]] double calculateGlobalPopFactor(const PoliticalManager& politicalManager) const;
	[[nodiscard]] double calculateTerrainMultiplier(const std::shared_ptr<SubState>& subState) const;
	[[nodiscard]] double calculateStateTraitMultiplier(const std::shared_ptr<SubState>& subState) const;
	[[nodiscard]] double getDensityFactor(double perCapitaDev) const;

	void establishBureaucracy(const Country& country, const std::map<std::string, Law>& lawsMap, const Vic3DefinesLoader& defines) const;
	void hardcodePorts(const Country& country) const;
	void integrateHardcodedBuildings(const Country& country,
		 double defaultRatio,
		 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
		 const std::map<std::string, Law>& lawsMap,
		 const std::map<std::string, PopType>& popTypes,
		 MarketTracker& market) const;
	void distributeBudget(double globalCP, double totalIndustryScore) const;

	[[nodiscard]] std::vector<std::shared_ptr<SubState>> prepareSubStatesByBudget(const std::shared_ptr<Country>& country,
		 const std::map<std::string, Law>& lawsMap) const;
	void negotiateBuilding(const std::shared_ptr<SubState>& subState,
		 const std::map<std::string, std::shared_ptr<Sector>>& sectors,
		 const std::map<std::string, Law>& lawsMap,
		 const std::vector<std::shared_ptr<SubState>>& subStates,
		 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
		 const std::map<std::string, std::map<std::string, double>>& estimatedOwnershipFracs,
		 double defaultRatio,
		 MarketTracker& marketTracker) const;
	[[nodiscard]] static std::shared_ptr<Sector> getSectorWithMostBudget(const std::map<std::string, std::shared_ptr<Sector>>& sectors);
	void buildBuilding(const std::shared_ptr<Building>& building,
		 const std::shared_ptr<SubState>& subState,
		 const std::shared_ptr<Sector>& sector,
		 const std::map<std::string, Law>& lawsMap,
		 const std::vector<std::shared_ptr<SubState>>& subStates,
		 const std::map<std::string, std::tuple<int, double>>& estimatedPMs,
		 const std::map<std::string, std::map<std::string, double>>& estimatedOwnershipFracs,
		 double defaultRatio,
		 MarketTracker& marketTracker) const;
	void removeSubStateIfFinished(std::vector<std::shared_ptr<SubState>>& subStates,
		 const std::vector<std::shared_ptr<SubState>>::iterator& subState,
		 const std::map<std::string, Law>& lawsMap) const;

	[[nodiscard]] std::map<std::string, int> apportionInvestors(int levels,
		 const std::map<std::string, double>& investorWeights,
		 std::map<std::string, double>& investorIOUs) const;
	[[nodiscard]] std::map<std::string, std::map<std::string, double>> estimateInvestorBuildings(const Country& country) const;

	void loadTerrainModifierMatrices(const std::string& filePath = "");
	void loadStateTraits(const commonItems::ModFilesystem& modFS);
	void loadBuildingInformation(const commonItems::ModFilesystem& modFS);
	void loadBuildingMappings(const std::string& filePath = "");
	void loadPMMappings(const std::string& filePath = "");
	void loadEconDefines(const std::string& filePath = "");
	void loadNationalBudgets(const std::string& filePath = "");
	void loadOwnerships(const std::string& filePath = "");
	void loadTechMap(const commonItems::ModFilesystem& modFS);
	void loadDemand(const commonItems::ModFilesystem& modFS);
	void loadPopTypes(const commonItems::ModFilesystem& modFS);


	std::vector<std::shared_ptr<Country>> centralizedCountries;

	EconDefinesLoader econDefines;
	NationalBudgetLoader nationalBudgets;
	mappers::ProductionMethodMapper PMMapper;
	OwnershipLoader ownershipLoader;

	TechLoader techMap;

	std::map<std::string, StateModifier> stateTraits;
	std::map<std::string, double> stateTerrainModifiers;
	std::map<std::string, std::map<std::string, double>> buildingTerrainModifiers;

	mappers::BuildingMapper buildingMapper;
	std::map<std::string, Building> buildings;
	BuildingGroups buildingGroups;
	std::map<std::string, ProductionMethod> PMs;
	std::map<std::string, ProductionMethodGroup> PMGroups;

	DemandLoader demand;
	PopTypeLoader popTypeLoader;
};
} // namespace V3


#endif // ECONOMY_MANAGER_H