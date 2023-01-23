#ifndef PROVINCE_MANAGER
#define PROVINCE_MANAGER
#include "BuildingCostLoader/BuildingCostLoader.h"
#include "DatingData.h"
#include "DefaultMapParser/DefaultMapParser.h"
#include "DefinitionScraper/DefinitionScraper.h"
#include "EU4Province.h"
#include "Parser.h"
#include "RegionManager/RegionManager.h"
#include <map>

namespace EU4
{
class ProvinceManager: commonItems::parser
{
  public:
	ProvinceManager() = default;
	void loadProvinces(std::istream& theStream);

	void loadParsers(const commonItems::ModFilesystem& modFS);

	// individual external loads for testing
	void loadDefaultMapParser(const DefaultMapParser& defaultParser) { defaultMapParser = defaultParser; }
	void loadDefinitionScraper(const DefinitionScraper& defScraper) { definitionScraper = defScraper; }
	void loadBuildingCostLoader(const BuildingCostLoader& buildCostLoader) { buildingCostLoader = buildCostLoader; }

	[[nodiscard]] const auto& getAllProvinces() const { return provinces; }
	[[nodiscard]] std::shared_ptr<Province> getProvince(int provinceID) const;
	[[nodiscard]] bool isProvinceWasteland(int provinceID) const { return wastelands.contains(provinceID); }
	[[nodiscard]] bool isProvinceDiscarded(int provinceID) const { return discardedProvinces.contains(provinceID); }

	void classifyProvinces(const RegionManager& regionManager);
	void buildPopRatios(const DatingData& datingData, bool convertAll);
	void buildProvinceWeights(const RegionManager& regionManager);

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<Province>> provinces;
	std::map<int, std::shared_ptr<Province>> wastelands;			 // Wastelands we DO care for but can't use as we'd use regular provinces.
	std::map<int, std::shared_ptr<Province>> discardedProvinces; // RNW, lakes etc. Stuff we know about but don't care for.

	DefaultMapParser defaultMapParser;
	DefinitionScraper definitionScraper;
	BuildingCostLoader buildingCostLoader;
};
} // namespace EU4

#endif // PROVINCE_MANAGER