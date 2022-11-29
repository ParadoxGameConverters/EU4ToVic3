#ifndef STATE_H
#define STATE_H
#include "ClayManager/ClayMapTypedefs.h"
#include "PopManager/Pops/StatePops.h"
#include <Parser.h>

namespace V3
{
class SubState;
struct ProvinceTypeCounter;
class State: commonItems::parser
{
  public:
	State() = default;
	void loadState(std::istream& theStream);
	void setStateName(const std::string& theName) { name = theName; }
	void addSubState(const std::shared_ptr<SubState>& substate) { substates.push_back(substate); }
	void distributeLandshares() const;
	void distributeResources();
	void setVanillaPops(const auto& thePops) { vanillaPops = thePops; }

	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] bool containsProvince(const std::string& provinceName) const { return provinces.contains(provinceName); }
	[[nodiscard]] std::shared_ptr<Province> getProvince(const std::string& provinceName) const;
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] bool isCoastal() const { return coastal; }
	[[nodiscard]] bool isSea() const;
	[[nodiscard]] bool isLake() const; // yes, an entire state can be a lake.
	[[nodiscard]] const auto& getSubStates() const { return substates; }
	[[nodiscard]] const auto& getTraits() const { return traits; }
	[[nodiscard]] const auto& getCappedResources() const { return cappedResources; }
	[[nodiscard]] const auto& getArableResources() const { return arableResources; }
	[[nodiscard]] const auto& getVanillaPops() const { return vanillaPops; }

  private:
	void registerKeys();

	static int calculateWeightedProvinceTotals(const ProvinceTypeCounter& theCount);
	static std::unique_ptr<ProvinceTypeCounter> countProvinceTypes(ProvinceMap provinces);

	bool coastal = false;
	std::string name;
	ProvinceMap provinces; // in xA2345A format
	std::vector<std::shared_ptr<SubState>> substates;
	std::vector<std::string> traits;				  // state_trait_natural_harbors
	std::map<std::string, int> cappedResources; // RGO and arable land potential
	std::vector<std::string> arableResources;	  // Which buildings can be built on arable land
	StatePops vanillaPops;
};
} // namespace V3
#endif // STATE_H