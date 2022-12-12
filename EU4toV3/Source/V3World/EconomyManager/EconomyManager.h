#ifndef ECONOMY_MANAGER_H
#define ECONOMY_MANAGER_H
#include "Configuration.h"

namespace V3
{
class Country;
/*
 * PreReqs: Clay(Substates merged under the right country), Pops, Laws, Tech
 * all must be converted first in the current design.
 *
 *
 * Primarily handle buildings, and maybe trade routes?
 * 1. For each centralized country get a CP budget based on population and Eurocentrism
 * 2. Read in terrain based effect Matrices
 * 3. For each substate in a centralized country get a CP budget based on population and terrain/state modifiers
 * 4. Figure out the "national budget" of each country based on eurocentrism, colonialism, GP status, EU4 gov reform, manufactories maybe?
 * 4b. National Budget is like 30% Agriculture, 25% Industry, etc.
 * 4c. Excludes Military, Navy, government administration, ports and rails
 * 5. Each Substate spends it's CP budget according to it's owners national budget and local terrain/state modifiers
 * 5b. When a substate goes over its infrastructure, try to build ports/rails. If you can't, go over the limit by up to 20%
 *
 * 6 Millitary and navy
 *
 * 7. Trade routes?
 * 7b. If ports are needed for trade build more ports?
 *
 * 8. Figured out required bureaucracy, depends on tech, institutions, trade routes and maybe laws
 */
class EconomyManager
{
  public:
	EconomyManager() = default;
	void loadCentralizedCountries(const std::map<std::string, std::shared_ptr<Country>>& countries);
	void assignCountryCPBudgets(Configuration::ECONOMY econmoyTrigger, const std::map<std::string, std::shared_ptr<Country>>& countries) const;
	void loadTerrainModifierMatrices() const;
	void assignSubStateCPBudgets() const;
	void balanceNationalBudgets() const;
	void buildBuildings() const;

	void convertArmy() const;
	void convertNavy() const;

	void generateTrade() const;
	void backfillBureaucracy() const;


  private:
	int getCentralizedWorldPopCount() const;
	int getWorldPopCount(const std::map<std::string, std::shared_ptr<Country>>& countries) const;
	double calculatePopDistanceFactor(int countryPopCount, int centralizedPopCount) const;

	std::vector<std::shared_ptr<Country>> centralizedCountries;
};
} // namespace V3


#endif // ECONOMY_MANAGER_H