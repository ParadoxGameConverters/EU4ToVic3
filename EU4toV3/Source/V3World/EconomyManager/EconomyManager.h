#ifndef ECONOMY_MANAGER_H
#define ECONOMY_MANAGER_H
#include "Configuration.h"
#include "Loaders/DefinesLoader/EconDefinesLoader.h"
#include "PoliticalManager/PoliticalManager.h"

namespace V3
{
class StateModifier;
class Country;
class Building;
class BuildingGroups;
class ProductionMethod;
class ProductionMethodGroup;
/*
 * PreReqs: Clay(Substates merged under the right country), Pops, Laws, Tech
 * all must be converted first in the current design.
 *
 *
 * Primarily handles buildings
 * 1. Load in centralized States
 * 2. Read in Mappers & Configs
 * 3. Bureaucracy! Have to handle it separate for game balance. Hand out generation that ~ matches need
 * 4. For each centralized country get a CP budget based on fronter option
 * 5. For each substate in a centralized country get a CP budget based on fronter option and terrain/state modifiers
 * 6. Figure out the "national budget" of each country using the sector blueprints in NationalBudgetLoader
 * 6b. National Budget is a list of sectors like 30% Farming, 25% Light Industry, etc.
 * 6c. Each Sector has a list of buildings that fall under it
 * 7. Each Substate scores buildings based on EU4 buildings, terrain, and state modifiers
 * 7b. Only buildings that are valid (resource/tech-wise) are scored
 * 8. A negotiation between the states and their country about what to build
 * 8b. The state w/ the most CP asks to build it's highest scoring building
 * 8c. The country says that building is in sector A and as a country we have X CP to spend in that sector
 * 8d. The state then builds as many buildings of that kind as it can, limited by capacity, packet size and sector CP
 * 8e. That state then re-scores the building it just built adjusting for the satisfied need
 * 8f. A bunch of small details that make this flow until all CP is assigned. Repeat for each country.
 *
 *
 * This *should* give nice diverse, sensible, game-balanced economies while maintaining a reasonable configuration footprint.
 * There's probably easier ways to do it, I couldn't think of one that didn't sacrifice aspects I cared about.
 */
class EconomyManager
{
  public:
	EconomyManager() = default;
	void loadCentralizedStates(const std::map<std::string, std::shared_ptr<Country>>& countries);
	void loadMappersAndConfigs(const commonItems::ModFilesystem& modFS, const std::string& filePath = "");

	void establishBureaucracy(const PoliticalManager& politicalManager) const;
	void hardcodePorts() const; // very temporary

	void assignCountryCPBudgets(Configuration::ECONOMY economyType, const PoliticalManager& politicalManager) const;
	void assignSubStateCPBudgets(Configuration::ECONOMY economyType) const;
	void balanceNationalBudgets() const;
	void planSubStateEconomies() const;
	void buildBuildings() const;

	[[nodiscard]] const auto& getCentralizedCountries() const { return centralizedCountries; }

  private:
	static double calculatePopDistanceFactor(int countryPopulation, double geoMeanPopulation);

	[[nodiscard]] double calculateGeoMeanCentralizedPops() const;
	[[nodiscard]] std::optional<std::string> pickBureaucracyPM(const std::shared_ptr<Country>& country) const;
	[[nodiscard]] double calculateGlobalPopFactor(const PoliticalManager& politicalManager) const;
	[[nodiscard]] double calculateTerrainMultiplier(const std::shared_ptr<SubState>& subState) const;
	[[nodiscard]] double calculateStateTraitMultiplier(const std::shared_ptr<SubState>& subState) const;

	void distributeBudget(double globalCP, double totalIndustryScore) const;
	void setPMs() const;

	void loadTerrainModifierMatrices(const std::string& filePath = "");
	void loadStateTraits(const commonItems::ModFilesystem& modFS);
	void loadBuildingInformation(const commonItems::ModFilesystem& modFS);
	void loadEconDefines(const std::string& filePath = "");


	std::vector<std::shared_ptr<Country>> centralizedCountries;

	EconDefinesLoader econDefines;

	std::map<std::string, std::shared_ptr<StateModifier>> stateTraits;
	std::map<std::string, double> stateTerrainModifiers;
	std::map<std::string, std::map<std::string, double>> buildingTerrainModifiers;

	std::map<std::string, std::shared_ptr<Building>> buildings;
	std::shared_ptr<BuildingGroups> buildingGroups;
	std::map<std::string, std::shared_ptr<ProductionMethod>> PMs;
	std::map<std::string, std::shared_ptr<ProductionMethodGroup>> PMGroups;
};
} // namespace V3


#endif // ECONOMY_MANAGER_H