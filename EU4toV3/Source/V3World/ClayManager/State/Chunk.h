#ifndef V3_CHUNK_H
#define V3_CHUNK_H
#include "PoliticalManager/Country/Country.h"
#include "PopManager/Demographic.h"
#include "Province.h"
#include "State.h"
#include <EU4World/CountryManager/EU4Country.h>
#include <map>
#include <memory>
#include <string>

/* A Chunk is a collection of provinces defined by province_mappings.txt where a particular eu4 province maps to.
 * Its defining attribute is the single owner these provinces belong to. From Chunks we can generate substates, when
 * we overlay this political map over a geographical map.
 *
 * A chunk can be as small as a single V3 province, or as large as multiple V3 states.*/

namespace V3
{
class Chunk
{
  public:
	Chunk() = default;

	void addSourceProvince(const std::pair<int, std::shared_ptr<EU4::Province>>& province) { sourceProvinces.emplace(province); }
	void addProvince(const std::pair<std::string, std::shared_ptr<Province>>& province) { provinces.emplace(province); }
	void setSourceOwner(const std::shared_ptr<EU4::Country>& theOwner) { sourceOwner = theOwner; }
	void setOwner(const std::shared_ptr<Country>& theOwner) { owner = theOwner; }
	void addState(const std::pair<std::string, std::shared_ptr<State>>& state) { states.emplace(state); }
	void addSubState(const std::shared_ptr<SubState>& subState) { substates.push_back(subState); }

	[[nodiscard]] const auto& getSourceProvinces() const { return sourceProvinces; }
	[[nodiscard]] const auto& getProvinces() const { return provinces; }
	[[nodiscard]] const auto& getSourceOwner() const { return sourceOwner; }
	[[nodiscard]] const auto& getOwner() const { return owner; }
	[[nodiscard]] const auto& getStates() const { return states; }
	[[nodiscard]] const auto& getSubStates() const { return substates; }

	void importDemographics();
	[[nodiscard]] const auto& getDemographics() const { return demographics; }

  private:
	void createDemographics(const std::vector<EU4::PopRatio>& popRatios, double provinceWeightRatio);
	void copyDemographicsToSubStates() const;

	std::map<int, std::shared_ptr<EU4::Province>> sourceProvinces; // EU4 provinces
	std::map<std::string, std::shared_ptr<Province>> provinces;		// V3 provinces
	std::shared_ptr<EU4::Country> sourceOwner;
	std::shared_ptr<Country> owner;
	std::map<std::string, std::shared_ptr<State>> states;
	std::vector<std::shared_ptr<SubState>> substates; // substates this chunk breaks down into

	std::vector<Demographic> demographics;
};
} // namespace V3

#endif // V3_CHUNK_H
