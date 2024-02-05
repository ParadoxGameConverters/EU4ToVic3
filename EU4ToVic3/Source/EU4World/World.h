#ifndef EU4_WORLD_H
#define EU4_WORLD_H
#include "Configuration.h"
#include "ConverterVersion.h"
#include "CountryManager/EU4CountryManager.h"
#include "CultureLoader/CultureLoader.h"
#include "DatingData.h"
#include "DiplomacyParser/DiplomacyParser.h"
#include "GameVersion.h"
#include "ModLoader/ModLoader.h"
#include "Parser.h"
#include "ProvinceManager/ProvinceManager.h"
#include "RegionManager/RegionManager.h"
#include "ReligionLoader/ReligionLoader.h"
#include "WarParser/WarParser.h"

namespace EU4
{
class World: commonItems::parser
{
  public:
	World(const std::shared_ptr<Configuration>& theConfiguration, const commonItems::ConverterVersion& converterVersion);

	// V3World inputs
	[[nodiscard]] bool isHREDecentralized() const { return hreReforms.contains("emperor_reichskrieg"); }
	[[nodiscard]] auto getEU4RandomSeed() const { return eu4Seed; } // for use in province shuffler

	[[nodiscard]] const auto& getWars() const { return wars; }

	[[nodiscard]] const auto& getCountryManager() const { return countryManager; }
	[[nodiscard]] const auto& getProvinceManager() const { return provinceManager; }
	[[nodiscard]] const auto& getRegionManager() const { return regionManager; }
	[[nodiscard]] const auto& getCultureLoader() const { return cultureLoader; }
	[[nodiscard]] const auto& getReligionLoader() const { return religionLoader; }
	[[nodiscard]] const auto& getDiplomacy() const { return diplomacyParser; }
	[[nodiscard]] const auto& getEU4Localizations() const { return countryManager.getLocalizationLoader(); }
	[[nodiscard]] const auto& getDatingData() const { return datingData; }
	[[nodiscard]] const auto& getEU4ModFS() const { return modFS; }

  private:
	void registerKeys(const std::shared_ptr<Configuration>& theConfiguration, const commonItems::ConverterVersion& converterVersion);
	void verifySave();

	struct saveData
	{
		bool parsedMeta = false;
		std::string metadata;
		std::string gamestate;
		std::string path;
	} saveGame;

	std::string EU4Path;
	Mods mods;
	commonItems::ModFilesystem modFS;

	DatingData datingData;

	GameVersion version;
	int eu4Seed = 0;
	std::string HREmperor;
	std::string celestialEmperor;
	std::set<std::string> hreReforms;
	std::string revolutionTarget;
	std::vector<WarParser> wars;

	RegionManager regionManager;
	ReligionLoader religionLoader;
	CultureLoader cultureLoader;
	ProvinceManager provinceManager;
	CountryManager countryManager;
	DiplomacyParser diplomacyParser;
};
} // namespace EU4

#endif // EU4_WORLD_H
