#ifndef PROVINCE_MANAGER
#define PROVINCE_MANAGER
#include "BuildingCostLoader/BuildingCostLoader.h"
#include "DatingData.h"
#include "DefaultMapParser/DefaultMapParser.h"
#include "DefinitionScraper/DefinitionScraper.h"
#include "EU4Province.h"
#include "ModLoader/ModLoader.h"
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

	void loadParsers(const std::string& EU4Path, const Mods& mods);

	// individual external loads for testing
	void loadDefaultMapParser(const DefaultMapParser& defaultParser) { defaultMapParser = defaultParser; }
	void loadDefinitionScraper(const DefinitionScraper& defScraper) { definitionScraper = defScraper; }
	void loadBuildingCostLoader(const BuildingCostLoader& buildCostLoader) { buildingCostLoader = buildCostLoader; }

	[[nodiscard]] const auto& getAllProvinces() const { return provinces; }
	[[nodiscard]] std::shared_ptr<Province> getProvince(int provinceID) const;

	void classifyProvinces(const RegionManager& regionManager);
	void buildPopRatios(const DatingData& datingData, bool convertAll);
	void buildProvinceWeights();

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<Province>> provinces;

	DefaultMapParser defaultMapParser;
	DefinitionScraper definitionScraper;
	BuildingCostLoader buildingCostLoader;
};
} // namespace EU4

#endif // PROVINCE_MANAGER