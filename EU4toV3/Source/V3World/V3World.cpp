#include "V3World.h"
#include "EU4World/World.h"
#include "Log.h"

V3::World::World(const Configuration& configuration, const EU4::World& sourceWorld): V3Path(configuration.getVic3Path()), configBlock(configuration.configBlock)
{
	Log(LogLevel::Progress) << "45 %";
	Log(LogLevel::Info) << "* Soaking up the shine *";
	clayManager.initializeVanillaStates(V3Path);
	clayManager.loadTerrainsIntoProvinces(V3Path);
	clayManager.initializeSuperRegions(V3Path);
	clayManager.loadStatesIntoSuperRegions();
	provinceMapper.loadProvinceMappings("configurables/province_mappings.txt");
	clayManager.generateChunks(provinceMapper, sourceWorld.getProvinceManager());
	clayManager.unDisputeChunkOwnership(sourceWorld.getCountryManager().getCountries());
	clayManager.distributeChunksAcrossSubStates();
	countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("configurables/country_mappings.txt");
	politicalManager.initializeVanillaCountries(V3Path);
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(sourceWorld.getCountryManager().getCountries());
	politicalManager.importVanillaCountries();
	clayManager.assignSubStateOwnership(politicalManager.getCountries(), *countryMapper);
	religionMapper.loadMappingRules("configurables/religion_map.txt");
	religionMapper.expandReligionMappings(sourceWorld.getReligionLoader().getAllReligions());
	cultureMapper.loadMappingRules("configurables/culture_map.txt");
	cultureMapper.expandCulturalMappings(clayManager, sourceWorld.getCultureLoader(), sourceWorld.getReligionLoader());
	clayManager.initializeVanillaPops(V3Path);

	Log(LogLevel::Info) << "*** Hello Vicky 3, creating world. ***";
	Log(LogLevel::Info) << "-> Importing Provinces";
	Log(LogLevel::Progress) << "46 %";

	Log(LogLevel::Info) << "-> Importing Vanilla Pops";
	Log(LogLevel::Progress) << "47 %";

	Log(LogLevel::Info) << "-> Importing Potential Countries";
	Log(LogLevel::Progress) << "48 %";

	Log(LogLevel::Info) << "-> Loading Country Mapping Rules";
	Log(LogLevel::Progress) << "49 %";

	Log(LogLevel::Info) << "-> Loading Culture Mapping Rules";
	Log(LogLevel::Progress) << "50 %";

	Log(LogLevel::Info) << "-> Pouring From Hollow Into Empty";
	Log(LogLevel::Progress) << "51 %";

	Log(LogLevel::Info) << "-> Converting Countries";
	Log(LogLevel::Progress) << "52 %";

	Log(LogLevel::Info) << "-> Converting Provinces";
	Log(LogLevel::Progress) << "53 %";

	Log(LogLevel::Info) << "-> Cataloguing Invasive Fauna";
	Log(LogLevel::Progress) << "54 %";

	Log(LogLevel::Info) << "-> Converting Diplomacy";
	Log(LogLevel::Progress) << "55 %";

	Log(LogLevel::Info) << "-> Setting Up States";
	Log(LogLevel::Progress) << "57 %";

	Log(LogLevel::Info) << "-> Generating Unciv Reforms";
	Log(LogLevel::Progress) << "58 %";

	Log(LogLevel::Info) << "-> Converting Technology Levels";
	Log(LogLevel::Progress) << "59 %";

	Log(LogLevel::Info) << "-> Distributing Factories";
	Log(LogLevel::Progress) << "60 %";

	Log(LogLevel::Info) << "-> Distributing Pops";
	Log(LogLevel::Progress) << "61 %";

	Log(LogLevel::Info) << "-> Releasing Invasive Fauna Into Colonies";
	Log(LogLevel::Progress) << "62 %";

	Log(LogLevel::Info) << "-> Monitoring Native Fauna Reaction";
	Log(LogLevel::Progress) << "63 %";

	Log(LogLevel::Info) << "-> Dropping Infected AI Cores";
	Log(LogLevel::Progress) << "64 %";

	Log(LogLevel::Info) << "-> Dropping Poorly-Shaped States";
	Log(LogLevel::Progress) << "65 %";

	Log(LogLevel::Info) << "-> Merging Nations";
	Log(LogLevel::Info) << "-> Invoking the Undead";
	Log(LogLevel::Progress) << "66 %";

	Log(LogLevel::Info) << "-> Converting Armies and Navies";
	Log(LogLevel::Progress) << "67 %";

	Log(LogLevel::Info) << "-> Converting Ongoing Conflicts";
	Log(LogLevel::Progress) << "68 %";

	Log(LogLevel::Info) << "-> Describing Religion";
	Log(LogLevel::Progress) << "69 %";

	Log(LogLevel::Info) << "-> Converting Botanical Definitions";
	Log(LogLevel::Progress) << "70 %";

	Log(LogLevel::Info) << "-> Converting country flags";
	Log(LogLevel::Progress) << "71 %";

	Log(LogLevel::Info) << "*** Goodbye, Vicky 3, and godspeed. ***";
}
