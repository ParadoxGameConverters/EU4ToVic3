#ifndef EU4_WORLD_H
#define EU4_WORLD_H
#include "Configuration.h"
#include "ConverterVersion/ConverterVersion.h"
#include "CultureLoader/CultureLoader.h"
#include "DatingData.h"
#include "GameVersion.h"
#include "ModLoader/ModLoader.h"
#include "Parser.h"
#include "ProvinceManager/ProvinceManager.h"
#include "RegionManager/RegionManager.h"
#include "ReligionLoader/ReligionLoader.h"
#include "CountryManager/EU4CountryManager.h"

namespace EU4
{
class World: commonItems::parser
{
  public:
	World(const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion);

  private:
	void registerKeys(const Configuration& theConfiguration, const mappers::ConverterVersion& converterVersion);

	void verifySave();
	void verifySaveContents();
	bool uncompressSave();

	struct saveData
	{
		bool compressed = false;
		std::string metadata;
		std::string gamestate;
		std::string path;
	} saveGame;

	std::string EU4Path;
	Mods mods;

	DatingData datingData;

	GameVersion version;
	int eu4Seed = 0;

	RegionManager regionManager;
	ReligionLoader religionLoader;
	CultureLoader cultureLoader;
	ProvinceManager provinceManager;
	CountryManager countryManager;
};
} // namespace EU4

#endif // EU4_WORLD_H
