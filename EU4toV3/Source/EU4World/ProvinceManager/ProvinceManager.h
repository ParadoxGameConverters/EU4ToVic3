#ifndef PROVINCE_MANAGER
#define PROVINCE_MANAGER
#include "EU4Province.h"
#include "Mods/ModLoader.h"
#include "Parser.h"
#include <map>
#include "DefaultMapParser.h"
#include "DefinitionScraper.h"
#include "RegionManager/RegionManager.h"

namespace EU4
{
class ProvinceManager: commonItems::parser
{
  public:
	ProvinceManager() = default;
	void loadProvinces(std::istream& theStream);
	
	void loadParsers(const std::string& EU4Path, const Mods& mods);
	void loadParsers(const DefaultMapParser& defaultParser, const DefinitionScraper& defScraper)
	{
		defaultMapParser = defaultParser;
		definitionScraper = defScraper;
	} // used for testing.

	[[nodiscard]] const auto& getAllProvinces() const { return provinces; }
	[[nodiscard]] const std::shared_ptr<Province>& getProvince(int provinceID) const;

	void classifyProvinces(const RegionManager& regionManager);

  private:
	void registerKeys();

	std::map<int, std::shared_ptr<Province>> provinces;

	DefaultMapParser defaultMapParser;
	DefinitionScraper definitionScraper;
};
} // namespace EU4

#endif // PROVINCE_MANAGER