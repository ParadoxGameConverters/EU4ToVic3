#include "V3World.h"
#include "EU4World/World.h"
#include "Log.h"

V3::World::World(const Configuration& configuration, const EU4::World& sourceWorld):
	 V3Path(configuration.getVic3Path()), configBlock(configuration.configBlock), datingData(sourceWorld.getDatingData())
{
	Mods overrideMods;
	// We use decentralized world mod to fill out wasteland and out-of-scope clay with decentralized tribes.
	if (!configuration.configBlock.vn)
		overrideMods.emplace_back(Mod{"Decentralized World", "configurables/decentralized_world/"});
	const auto vanillaFS = commonItems::ModFilesystem(V3Path, {});
	const auto dwFS = commonItems::ModFilesystem(V3Path, overrideMods);
	overrideMods.emplace_back(Mod{"Blankmod", "blankMod/output/"});
	const auto allFS = commonItems::ModFilesystem(V3Path, overrideMods);
	overrideMods.clear();
	overrideMods.emplace_back(Mod{"Blankmod", "blankMod/output/"});
	const auto blankModFS = commonItems::ModFilesystem(V3Path, overrideMods);

	Log(LogLevel::Progress) << "45 %";
	Log(LogLevel::Info) << "* Soaking up the shine *";
	clayManager.loadAdjacencies("configurables/province_adjacencies.txt");
	clayManager.initializeVanillaStates(dwFS);
	clayManager.loadTerrainsIntoProvinces(dwFS);
	clayManager.initializeSuperRegions(dwFS);
	clayManager.loadStatesIntoSuperRegions();
	if (configuration.configBlock.vn)
	{
		provinceMapper.loadProvinceMappings("configurables/vn_province_mappings.txt");
		clayManager.loadVNColonialRules("configurables/vn_colonial.txt");
	}
	else
	{
		provinceMapper.loadProvinceMappings("configurables/province_mappings.txt");
	}
	countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("configurables/country_mappings.txt");
	religionMapper.loadMappingRules("configurables/religion_map.txt");
	religionMapper.expandReligionMappings(sourceWorld.getReligionLoader().getAllReligions());
	religionMapper.generateReligionDefinitions(dwFS,
		 "configurables/religion_group_map.txt",
		 sourceWorld.getReligionLoader().getAllReligions(),
		 sourceWorld.getEU4Localizations());
	cultureMapper.loadMappingRules("configurables/culture_map.txt");
	cultureMapper.loadColonialRules("configurables/colonial_regions.txt");
	cultureMapper.loadWesternizationRules("configurables/westernization.txt");
	politicalManager.loadPopulationSetupMapperRules("configurables/population_setup.txt");
	politicalManager.loadIdeaEffectMapperRules("configurables/idea_effects.txt");
	politicalManager.loadTechSetupMapperRules("configurables/tech_setup.txt");
	politicalManager.loadLawMapperRules("configurables/law_map.txt");
	politicalManager.loadLawDefinitions(dwFS);
	politicalManager.loadDiplomaticMapperRules("configurables/diplomatic_map.txt");
	politicalManager.loadCharacterTraitMapperRules("configurables/character_traits.txt");
	politicalManager.loadColonialTagMapperRules("configurables/colonial_tags.txt");
	politicalManager.loadCountryTierMapperRules("configurables/country_tiers.txt");
	politicalManager.loadMajorFormables("blankMod/output/common/country_formation/00_major_formables.txt");
	politicalManager.loadIGIdeologiesMapperRules("configurables/ig_ideologies.txt");
	politicalManager.loadAIStrategies("configurables/ai_strategy_map.txt");
	politicalManager.loadAISecretGoals("configurables/ai_secret_goal_map.txt");
	popManager.loadMinorityPopRules("configurables/minority_pops.txt");
	popManager.loadSlaveCultureRules("configurables/slave_culture_map.txt");
	cultureMapper.expandCulturalMappings(clayManager, sourceWorld.getCultureLoader(), sourceWorld.getReligionLoader());
	localizationLoader.scrapeLocalizations(dwFS);
	vanillaLocalizationLoader.scrapeLocalizations(blankModFS);
	cultureMapper.loadCultureDefinitions(allFS);
	cultureMapper.loadTraitDefinitions(allFS);
	economyManager.loadMappersAndConfigs(allFS);

	Log(LogLevel::Info) << "*** Hello Vicky 3, creating world. ***";
	Log(LogLevel::Progress) << "46 %";
	// chunks and substate generation
	clayManager.generateChunks(provinceMapper, sourceWorld.getProvinceManager());
	clayManager.unDisputeChunkOwnership(sourceWorld.getCountryManager().getCountries());
	clayManager.splitChunksIntoSubStates();

	Log(LogLevel::Progress) << "47 %";
	// initializing countries from eu4 and vanilla
	politicalManager.initializeVanillaCountries(dwFS, vanillaFS);
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(sourceWorld.getCountryManager().getCountries());

	Log(LogLevel::Progress) << "48 %";
	// assigning clay to countries
	clayManager.assignSubStateOwnership(politicalManager.getCountries(), *countryMapper);

	Log(LogLevel::Progress) << "49 %";
	// soaking up vanilla pops
	popManager.initializeVanillaPops(vanillaFS);
	popManager.initializeDWPops(dwFS);
	popManager.injectReligionsIntoVanillaPops(cultureMapper.getV3CultureDefinitions());
	popManager.injectReligionsIntoDWPops(cultureMapper.getV3CultureDefinitions());

	// inject vanilla substates into map holes.
	clayManager.injectVanillaSubStates(dwFS, politicalManager, popManager, configBlock.vn);

	Log(LogLevel::Progress) << "50 %";
	// handling demographics
	popManager.convertDemographics(clayManager, cultureMapper, religionMapper, sourceWorld.getCultureLoader(), sourceWorld.getReligionLoader());

	Log(LogLevel::Progress) << "51 %";
	// generating decentralized countries
	clayManager.shoveRemainingProvincesIntoSubStates();
	politicalManager.generateDecentralizedCountries(clayManager, popManager);

	Log(LogLevel::Progress) << "52 %";
	// converting all 3 types of countries - generated decentralized, extinct/extant-VN vanilla-only, and EU4 imports.
	politicalManager.convertAllCountries(clayManager,
		 cultureMapper,
		 religionMapper,
		 sourceWorld.getCultureLoader(),
		 sourceWorld.getReligionLoader(),
		 localizationLoader,
		 sourceWorld.getEU4Localizations(),
		 configBlock.downTiers,
		 configBlock.vn);

	Log(LogLevel::Progress) << "53 %";
	politicalManager.attemptColonialTagReplacement(cultureMapper.getColonialRegionMapper(), clayManager);

	Log(LogLevel::Progress) << "54 %";
	popManager.generatePops(clayManager, configBlock.popShaping);
	popManager.applyHomeLands(clayManager);

	Log(LogLevel::Progress) << "55 %";
	cultureMapper.generateCultureDefinitions("configurables/name_lists.txt",
		 "configurables/name_list_map.txt",
		 "configurables/culture_trait_map.txt",
		 clayManager,
		 sourceWorld.getCultureLoader(),
		 sourceWorld.getReligionLoader(),
		 sourceWorld.getEU4Localizations());

	Log(LogLevel::Progress) << "56 %";
	politicalManager.determineAndApplyWesternization(cultureMapper, religionMapper, configBlock.euroCentric, configBlock.startDate, datingData);
	Log(LogLevel::Progress) << "57 %";
	politicalManager.setupTech();
	Log(LogLevel::Progress) << "58 %";
	politicalManager.setupLaws();
	politicalManager.convertDiplomacy(sourceWorld.getDiplomacy().getAgreements());
	politicalManager.convertRivals();
	politicalManager.convertTruces(datingData.lastEU4Date);
	if (configuration.configBlock.vn)
	{
		politicalManager.importVNColonialDiplomacy(clayManager);
		politicalManager.importVanillaDiplomacy();
	}

	Log(LogLevel::Progress) << "59 %";
	clayManager.squashAllSubStates(politicalManager);
	Log(LogLevel::Progress) << "60 %";
	clayManager.redistributeResourcesAndLandshares();

	Log(LogLevel::Progress) << "61 %";
	cultureMapper.injectReligionsIntoCultureDefs(clayManager);
	Log(LogLevel::Progress) << "62 %";
	politicalManager.convertCharacters(datingData.lastEU4Date,
		 configBlock.startDate,
		 clayManager,
		 cultureMapper,
		 religionMapper,
		 sourceWorld.getCultureLoader(),
		 sourceWorld.getReligionLoader());

	Log(LogLevel::Progress) << "63 %";
	flagCrafter.loadCustomColors(configuration.getEU4Path() + "/common/custom_country_colors/00_custom_country_colors.txt");
	flagCrafter.loadAvailableFlags("blankMod/output/common/coat_of_arms/coat_of_arms/", V3Path + "/common/flag_definitions/");
	Log(LogLevel::Progress) << "64 %";
	flagCrafter.distributeAvailableFlags(politicalManager.getCountries(), *countryMapper);

	Log(LogLevel::Progress) << "65 %";
	politicalManager.injectDynamicCulturesIntoFormables(cultureMapper);
	Log(LogLevel::Progress) << "66 %";
	politicalManager.expandReleasablesFootprint(clayManager);
	politicalManager.alterReleasablesReligion();
	Log(LogLevel::Progress) << "67 %";
	politicalManager.alterMajorFormables();
	Log(LogLevel::Progress) << "68 %";
	clayManager.filterInvalidClaims(politicalManager);
	Log(LogLevel::Progress) << "69 %";
	politicalManager.alterIGIdeologies(cultureMapper, religionMapper, clayManager);
	politicalManager.generateAIStrategies(clayManager);
	politicalManager.generateAISecretGoals(clayManager);
	Log(LogLevel::Progress) << "70 %";
	popManager.liberateSlaves(politicalManager);
	popManager.alterSlaveCultures(politicalManager, clayManager, cultureMapper.getV3CultureDefinitions());

	politicalManager.incorporateStates(cultureMapper, clayManager);
	politicalManager.designateTreatyPorts(clayManager);
	politicalManager.distributeColonialClaims(clayManager);

	// Place starting buildings for all centralized countries
	Log(LogLevel::Progress) << "71 %";
	economyManager.loadCentralizedStates(politicalManager.getCountries());
	Log(LogLevel::Progress) << "72 %";
	economyManager.establishBureaucracy(politicalManager);
	Log(LogLevel::Progress) << "73 %";
	economyManager.hardcodePorts();
	Log(LogLevel::Progress) << "74 %";
	economyManager.assignCountryCPBudgets(configBlock.economy, configBlock.startDate, datingData, politicalManager, configBlock.vn);
	economyManager.balanceNationalBudgets();
	Log(LogLevel::Progress) << "75 %";
	economyManager.assignSubStateCPBudgets(configBlock.economy);
	Log(LogLevel::Progress) << "76 %";
	economyManager.buildBuildings(politicalManager.getLawsMap());
	economyManager.setPMs();

	Log(LogLevel::Info) << "*** Goodbye, Vicky 3, and godspeed. ***";
}
