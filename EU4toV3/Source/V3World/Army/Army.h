#ifndef ARMY_H
#define ARMY_H

#include "../../EU4World/Army/EU4Army.h"
#include "../../Mappers/PortProvinces/PortProvinces.h"
#include "../../Mappers/ProvinceMappings/ProvinceMapper.h"
#include "../../Mappers/RegimentCosts/RegimentCostsMapper.h"
#include "../Province/Province.h"
#include "Regiment.h"

namespace V2
{
enum class AddRegimentToArmyResult
{
	success,
	retry,
	fail
};

class Army // also Navy
{
  public:
	Army(const EU4::EU4Army& eu4Army,
		 std::string _tag,
		 bool civilized,
		 const mappers::RegimentCostsMapper& regimentCostsMapper,
		 std::map<int, std::shared_ptr<Province>> allProvinces,
		 const mappers::ProvinceMapper& provinceMapper,
		 const mappers::PortProvinces& portProvincesMapper,
		 std::map<REGIMENTTYPE, int>& unitNameCount,
		 const std::string& localAdjective);

	std::map<REGIMENTTYPE, double>& getArmyRemainders() { return armyRemainders; }
	[[nodiscard]] bool hasRegimentsOfType(REGIMENTTYPE chosenType) const;
	[[nodiscard]] double getRegimentRemainder(REGIMENTTYPE chosenType) const;
	[[nodiscard]] auto success() const { return created; }

	void addRegimentRemainder(const REGIMENTTYPE chosenType, const double value) { armyRemainders[chosenType] += value; }

	friend std::ostream& operator<<(std::ostream& output, const Army& army);

	AddRegimentToArmyResult addRegimentToArmy(REGIMENTTYPE chosenType,
		 const std::map<int, std::shared_ptr<Province>>& allProvinces,
		 const mappers::ProvinceMapper& provinceMapper,
		 const mappers::PortProvinces& portProvincesMapper,
		 std::map<REGIMENTTYPE, int>& unitNameCount,
		 const std::string& localAdjective);

	static std::set<int> getPortProvinces(const std::set<int>& locationCandidates,
		 std::map<int, std::shared_ptr<Province>> allProvinces,
		 const mappers::PortProvinces& portProvincesMapper);

  private:
	[[nodiscard]] std::optional<int> getProbabilisticHomeProvince(REGIMENTTYPE chosenType) const;
	void blockHomeProvince(int blocked);

	static REGIMENTTYPE pickCategory(const std::string& incCategory, bool civilized);
	static std::shared_ptr<Province> pickRandomPortProvince(const std::set<int>& homeCandidates,
		 const std::map<int, std::shared_ptr<Province>>& allProvinces);
	static bool provinceRegimentCapacityPredicate(std::shared_ptr<Province> prov1, std::shared_ptr<Province> prov2);
	static std::shared_ptr<Province> getProvinceForExpeditionaryArmy(const std::map<int, std::shared_ptr<Province>>& allProvinces, const std::string& tag);
	static std::string getRegimentName(REGIMENTTYPE chosenType, std::map<REGIMENTTYPE, int>& unitNameCount, const std::string& localAdjective);
	static int pickRandomProvinceID(std::set<int> homeCandidates);

	std::string name;
	int location = 0;
	std::vector<Regiment> regiments;
	std::map<std::string, int> regimentCounts;
	std::map<REGIMENTTYPE, int> buildOrder;
	std::map<REGIMENTTYPE, int> buildSuceeded;
	std::map<REGIMENTTYPE, double> armyRemainders;
	std::map<REGIMENTTYPE, std::vector<int>> eu4homeProvinces;
	bool isNavy = false;
	std::string tag;
	bool created = true;
};
} // namespace V2

#endif // ARMY_H
