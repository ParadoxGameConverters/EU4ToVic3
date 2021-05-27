#ifndef EU4_WORLD_H
#define EU4_WORLD_H
#include "Configuration.h"
#include "GameVersion.h"
#include "Mods/ModLoader.h"
#include "Parser.h"
#include "ConverterVersion/ConverterVersion.h"
#include "RegionManager/RegionManager.h"
#include "ReligionLoader/ReligionLoader.h"
#include "CultureLoader/CultureLoader.h"

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

	struct DatingData
	{
		date firstEU4Date; // first date we see in the save, usually predates startDate
		date startEU4Date; // savegame saved date - conversion date
		date lastEU4Date;	 // last date we see in the save, used to stop conversions
	};

	std::shared_ptr<DatingData> dating;

	GameVersion version;
	int eu4Seed = 0;

	RegionManager regionManager;
	ReligionLoader religionLoader;
	CultureLoader cultureLoader;
};
} // namespace EU4

#endif // EU4_WORLD_H
