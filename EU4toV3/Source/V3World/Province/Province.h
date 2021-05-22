#ifndef PROVINCE_H
#define PROVINCE_H

#include "../../EU4World/Country/EU4Country.h"
#include "../../EU4World/Provinces/EU4Province.h"
#include "../../Mappers/Geography/ClimateMapper.h"
#include "../../Mappers/Geography/TerrainDataMapper.h"
#include "../../Mappers/NavalBases/NavalBaseMapper.h"
#include "../../Mappers/ProvinceDetails/ProvinceDetailsMapper.h"
#include "../../Mappers/ProvinceMappings/ProvinceMapper.h"
#include "../Factory/Factory.h"
#include "../Pop/Pop.h"
#include "ProvinceNameParser.h"
#include <string>

namespace mappers
{
class CountryMappings;
class CultureMapper;
class ReligionMapper;
class Continents;
} // namespace mappers

namespace EU4
{
class Regions;
}

namespace V2
{
enum class CIV_ALGORITHM;
enum class REGIMENTTYPE;
class Country;

struct Demographic
{
	std::string culture;
	std::string slaveCulture;
	std::string religion;
	double upperRatio = 0.0;
	double middleRatio = 0.0;
	double lowerRatio = 0.0;
};

class Province
{
  public:
	Province(std::string _filename,
		 const mappers::ClimateMapper& climateMapper,
		 const mappers::TerrainDataMapper& terrainDataMapper,
		 const ProvinceNameParser& provinceNameParser,
		 const mappers::NavalBaseMapper& navalBaseMapper);

	void setRgoType(const std::string& _type) { details.rgoType = _type; }
	void setOwner(const std::string& _owner) { details.owner = _owner; }
	void setController(const std::string& _controller) { details.controller = _controller; }
	void setResettable() { resettable = true; }
	void clearCores() { details.cores.clear(); }
	void addVanillaPop(std::shared_ptr<Pop> vanillaPop);
	void addMinorityPop(std::shared_ptr<Pop> minorityPop);
	void setSlaveProportion(const double _pro) { slaveProportion = _pro; }
	void increaseRailLevel(const int level) { railLevel += level; }
	void setFortLevel(const int level) { fortLevel = level; }
	void setNavalBaseLevel(const int level) { navalBaseLevel = level; }
	void setColonial(const int colonyLevel) { colonial = colonyLevel; }
	void setTerritorialCore(const bool territory) { territorialCore = territory; }
	void addCore(const std::string& newCore);
	void addPopDemographic(const Demographic& d);
	void replaceCores(const std::set<std::string>& cores) { details.cores = cores; }

	[[nodiscard]] auto hasCore(const std::string& core) const { return details.cores.count(core) > 0; }
	[[nodiscard]] auto getID() const { return provinceID; }
	[[nodiscard]] auto isCoastal() const { return coastal; }
	[[nodiscard]] auto getResettable() const { return resettable; }
	[[nodiscard]] auto getProvinceWeight() const { return provinceWeight; }
	[[nodiscard]] auto isTerritorialCore() const { return territorialCore; }
	[[nodiscard]] auto hasNavalBase() const { return navalBaseLevel > 0; }
	[[nodiscard]] auto getNavalBaseLevel() const { return navalBaseLevel; }
	[[nodiscard]] auto getMfgCount() const { return mfgCount; }
	[[nodiscard]] auto isColony() const { return colonial != 0; }
	[[nodiscard]] auto getColonyLevel() const { return colonial; }
	[[nodiscard]] auto wasColony() const { return wasColonised; }
	[[nodiscard]] const auto& getEU4IDs() const { return eu4IDs; }
	[[nodiscard]] const auto& getRgoType() const { return details.rgoType; }
	[[nodiscard]] const auto& getClimate() const { return details.climate; }
	[[nodiscard]] const auto& getTerrain() const { return details.terrain; }
	[[nodiscard]] const auto& getName() const { return name; }
	[[nodiscard]] const auto& getOwner() const { return details.owner; }
	[[nodiscard]] const auto& getController() const { return details.controller; }
	[[nodiscard]] const auto& getFilename() const { return filename; }
	[[nodiscard]] const auto& getGeneratedNeoCultures() const { return generatedNeoCultures; }
	[[nodiscard]] const auto& getSuperRegion() const { return superRegion; }
	[[nodiscard]] const auto& getCores() const { return details.cores; }

	[[nodiscard]] std::string getDominantCulture();
	[[nodiscard]] int getTotalPopulation() const;
	[[nodiscard]] std::vector<std::string> getCulturesOverThreshold(double percentOfPopulation) const;
	[[nodiscard]] std::optional<std::pair<int, std::vector<std::shared_ptr<Pop>>>> getPopsForOutput() const;
	[[nodiscard]] std::vector<std::shared_ptr<Pop>> getPops(const std::string& type) const;
	[[nodiscard]] std::pair<int, int> getAvailableSoldierCapacity() const;

	std::string getRegimentName(REGIMENTTYPE chosenType);
	std::optional<std::shared_ptr<Factory>> addFactory(std::shared_ptr<Factory> factory);
	std::shared_ptr<Pop> getSoldierPopForArmy(bool force = false);

	void sterilizeProvince();
	void convertFromOldProvince(const std::vector<std::shared_ptr<EU4::Province>>& provinceSources,
		 const std::map<std::string, std::shared_ptr<EU4::Country>>& theEU4Countries,
		 const EU4::Regions& eu4Regions,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::CultureMapper& slaveCultureMapper,
		 const mappers::Continents& continentsMapper,
		 const mappers::ReligionMapper& religionMapper,
		 const mappers::CountryMappings& countryMapper,
		 const mappers::ProvinceMapper& provinceMapper,
		 bool hreDecentralized);
	void doCreatePops(double popWeightRatio, Country* _owner, CIV_ALGORITHM popConversionAlgorithm, const mappers::ProvinceMapper& provinceMapper);

	friend std::ostream& operator<<(std::ostream& output, const Province& province);

  private:
	int provinceID = 0;
	std::string name;
	std::string filename;
	int vanillaPopulation = 0;
	double slaveProportion = 0.0;
	bool coastal = false;
	bool resettable = false;
	bool territorialCore = false;
	bool inHRE = false;
	int colonial = 0;
	bool wasColonised = false;
	int fortLevel = 0;
	int navalBaseLevel = 0;
	int railLevel = 0;
	int mfgCount = 0;
	double investmentFactor = 0.0;
	double provinceWeight = 0.0;
	std::vector<std::shared_ptr<Pop>> vanillaPops;
	std::vector<std::shared_ptr<Pop>> minorityPops;
	std::vector<std::shared_ptr<Pop>> pops;
	std::map<std::string, std::shared_ptr<Factory>> factories;
	std::vector<Demographic> demographics;
	std::set<int> eu4IDs; // Source province IDs, fuzzy at best, use with care (might belong to whomever or be un-colonized).
	std::set<std::string> importedBuildings;
	std::set<std::string> importedIdeas;
	std::map<std::string, std::string> generatedNeoCultures; // originalCulture->neoculture accounting storage.
	std::string superRegion;											// orientation issue.
	std::map<REGIMENTTYPE, int> unitNameCount;
	mappers::ProvinceDetails details;

	struct pop_points
	{
		double craftsmen = 0;
		double slaves = 0;
		double soldiers = 0;
		double artisans = 0;
		double clergymen = 0;
		double clerks = 0;
		double bureaucrats = 0;
		double officers = 0;
		double capitalists = 0;
		double aristocrats = 0;
	};

	void determineDemographics(const EU4::Regions& eu4Regions,
		 std::vector<EU4::PopRatio>& popRatios,
		 int eu4ProvID,
		 const std::string& oldOwnerTag,
		 int destNum,
		 double provPopRatio,
		 mappers::CultureMapper& cultureMapper,
		 const mappers::CultureMapper& slaveCultureMapper,
		 const mappers::Continents& continentsMapper,
		 const mappers::ReligionMapper& religionMapper);
	pop_points getPopPoints_1(const Demographic& demographic, double newPopulation,
		 const Country* _owner) const; // EU4 1.0-1.11
	pop_points getPopPoints_2(const Demographic& demographic, double newPopulation,
		 const Country* _owner) const; // EU4 1.12 and newer
	void createPops(const Demographic& demographic,
		 double popWeightRatio,
		 const Country* _owner,
		 CIV_ALGORITHM popConversionAlgorithm,
		 const mappers::ProvinceMapper& provinceMapper);
	void combinePops();
	void determineColonial();
	static bool popSortBySizePredicate(std::shared_ptr<Pop> pop1, std::shared_ptr<Pop> pop2);
	static int getRequiredPopForRegimentCount(int count);
	bool growSoldierPop(Pop& pop);
};

std::ostream& operator<<(std::ostream& output, const std::optional<std::pair<int, std::vector<std::shared_ptr<Pop>>>>& pops);
} // namespace V2

#endif // PROVINCE_H
