#include "ClayManager/ClayManager.h"
#include "ClayManager/State/State.h"
#include "ClayManager/State/SubState.h"
#include "CountryManager/EU4Country.h"
#include "CountryMapper/CountryMapper.h"
#include "CultureLoader/CultureLoader.h"
#include "CultureMapper/CultureMapper.h"
#include "Loaders/LocLoader/LocalizationLoader.h"
#include "LocalizationLoader/EU4LocalizationLoader.h"
#include "PoliticalManager/Country/Country.h"
#include "PoliticalManager/PoliticalManager.h"
#include "PopManager/PopManager.h"
#include "ProvinceManager/ProvinceManager.h"
#include "ProvinceMapper/ProvinceMapper.h"
#include "ReligionLoader/ReligionLoader.h"
#include "ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
const auto eu4FS = commonItems::ModFilesystem("TestFiles/eu4installation/", {});

std::tuple<V3::PoliticalManager, V3::PopManager, mappers::CultureMapper, mappers::ReligionMapper, V3::ClayManager, EU4::CultureLoader, EU4::ReligionLoader>
prepMappers()
{
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(eu4FS);
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(eu4FS);
	EU4::RegionManager regionMapper;
	regionMapper.loadRegions(eu4FS);

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";																																		// sea, no ownership
	provinceStream << "-2={ owner = TA2 base_tax=10 base_production=10 base_manpower=10 culture = culture1 religion = catholic }\n"; // substate TA-2&3
	provinceStream << "-3={ owner = TA3 base_tax=1 base_production=1 base_manpower=1 culture = culture2 religion = protestant }\n";	// substate TA-2&3
	provinceStream << "-4={}\n";																																		// irrelevant
	provinceStream << "-5={}\n";																																		// irrelevant
	provinceStream << "-6={}\n";																																		// irrelevant
	provinceStream << "-7={}\n";																																		// irrelevant
	provinceStream << "-8={}\n";																																		// irrelevant
	provinceStream << "-9={ owner = TA9 base_tax=1 base_production=1 base_manpower=1 culture = culture3 religion = anglican }\n";		// substate TA-9
	provinceStream << "-10={}\n";																																		// irrelevant
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.loadDefaultMapParser(defaults);
	provinceManager.loadDefinitionScraper(definitions);
	provinceManager.classifyProvinces(regionMapper);
	provinceManager.buildProvinceWeights({});
	provinceManager.buildPopRatios({}, false);

	mappers::ProvinceMapper provinceMapper;
	provinceMapper.loadProvinceMappings("TestFiles/configurables/province_mappings_chunks.txt");

	V3::ClayManager clayManager;
	clayManager.initializeVanillaStates(modFS);
	clayManager.loadTerrainsIntoProvinces(modFS);
	clayManager.initializeSuperRegions(modFS);
	clayManager.loadStatesIntoSuperRegions();

	clayManager.generateChunks(provinceMapper, provinceManager);

	std::stringstream countryStream;
	const auto ta2 = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto ta3 = std::make_shared<EU4::Country>("TA3", countryStream);
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	const std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};
	clayManager.unDisputeChunkOwnership(countries);
	clayManager.splitChunksIntoSubStates();

	auto countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("TestFiles/configurables/country_mappings.txt");
	V3::PoliticalManager politicalManager;
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);
	clayManager.assignSubStateOwnership(politicalManager.getCountries(), *countryMapper);

	EU4::ReligionLoader religionLoader;
	religionLoader.loadReligions(eu4FS);
	EU4::CultureLoader cultureLoader;
	cultureLoader.loadCultures(eu4FS);

	mappers::ReligionMapper relMapper;
	relMapper.loadMappingRules("TestFiles/configurables/religion_map.txt");
	relMapper.expandReligionMappings(religionLoader.getAllReligions());

	mappers::CultureMapper culMapper;
	culMapper.loadMappingRules("TestFiles/configurables/culture_map.txt");
	culMapper.expandCulturalMappings(clayManager, cultureLoader, religionLoader);

	V3::PopManager popManager;
	popManager.initializeVanillaPops(modFS);
	popManager.initializeDWPops(modFS);
	popManager.convertDemographics(clayManager, culMapper, relMapper, cultureLoader, religionLoader);

	politicalManager.initializeVanillaCountries(modFS, modFS);
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);

	return std::tuple{politicalManager, popManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader};
}
} // namespace

TEST(V3World_PoliticalManagerTests, PoliticalManagerCanInitializeVanillaCountries)
{
	V3::PoliticalManager politicalManager;

	EXPECT_EQ(0, politicalManager.getCountries().size());

	politicalManager.initializeVanillaCountries(modFS, modFS);

	ASSERT_EQ(4, politicalManager.getCountries().size());

	const auto& country1 = politicalManager.getCountry("TAG");
	const auto& country2 = politicalManager.getCountry("GAT");
	const auto& country3 = politicalManager.getCountry("TGA");
	const auto& country4 = politicalManager.getCountry("CCC");

	EXPECT_EQ("TAG", country1->getTag());
	EXPECT_EQ("GAT", country2->getTag());
	EXPECT_EQ("TGA", country3->getTag());
	EXPECT_EQ("CCC", country4->getTag());

	EXPECT_FALSE(politicalManager.isTagDecentralized("TAG"));
	EXPECT_FALSE(politicalManager.isTagDecentralized("GAT"));
	EXPECT_FALSE(politicalManager.isTagDecentralized("TGA"));
	EXPECT_TRUE(politicalManager.isTagDecentralized("CCC"));
}

TEST(V3World_PoliticalManagerTests, PoliticalManagerCanImportCountries)
{
	const auto countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("TestFiles/configurables/country_mappings.txt");

	std::stringstream countryStream;
	const auto ta2 = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto ta3 = std::make_shared<EU4::Country>("TA3", countryStream);
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	const std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};

	V3::PoliticalManager politicalManager;
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);

	EXPECT_EQ(3, politicalManager.getCountries().size());

	const auto& country1 = politicalManager.getCountries().at("GA2");
	const auto& country2 = politicalManager.getCountries().at("X00");
	const auto& country3 = politicalManager.getCountries().at("GA9");

	EXPECT_EQ("TA2", country1->getSourceCountry()->getTag());
	EXPECT_EQ("TA3", country2->getSourceCountry()->getTag());
	EXPECT_EQ("TA9", country3->getSourceCountry()->getTag());
}

TEST(V3World_PoliticalManagerTests, PoliticalManagerCanConvertVanillaCountries)
{
	const V3::ClayManager clayManager;
	const EU4::EU4LocalizationLoader eu4LocLoader;
	const V3::LocalizationLoader v3LocLoader;

	const auto countryMapper = std::make_shared<mappers::CountryMapper>();
	countryMapper->loadMappingRules("TestFiles/configurables/country_mappings.txt");

	std::stringstream countryStream;
	const auto ta2 = std::make_shared<EU4::Country>("TA2", countryStream);
	const auto ta3 = std::make_shared<EU4::Country>("TA3", countryStream);
	const auto ta9 = std::make_shared<EU4::Country>("TA9", countryStream);
	const std::map<std::string, std::shared_ptr<EU4::Country>> countries = {{"TA2", ta2}, {"TA3", ta3}, {"TA9", ta9}};

	V3::PoliticalManager politicalManager;

	EXPECT_EQ(0, politicalManager.getCountries().size());
	politicalManager.initializeVanillaCountries(modFS, modFS);
	EXPECT_EQ(4, politicalManager.getCountries().size()); // we have 4 vanilla vic3 countries, unrelated to eu4.
	politicalManager.loadCountryMapper(countryMapper);
	politicalManager.importEU4Countries(countries);
	EXPECT_EQ(7, politicalManager.getCountries().size()); // we append the 3 imported eu4 countries.

	const auto& country1 = politicalManager.getCountries().at("GA2");
	const auto& country2 = politicalManager.getCountries().at("X00");
	const auto& country3 = politicalManager.getCountries().at("GA9");
	const auto& country4 = politicalManager.getCountries().at("TAG");
	const auto& country5 = politicalManager.getCountries().at("GAT");
	const auto& country6 = politicalManager.getCountries().at("TGA");
	const auto& country7 = politicalManager.getCountries().at("CCC");

	EXPECT_FALSE(country1->getProcessedData().color); // these 3 eu4 countries have no color.
	EXPECT_FALSE(country2->getProcessedData().color);
	EXPECT_FALSE(country3->getProcessedData().color);
	EXPECT_FALSE(country4->getProcessedData().color); // these 3 vanilla ones are also blank.
	EXPECT_FALSE(country5->getProcessedData().color);
	EXPECT_FALSE(country6->getProcessedData().color);
	EXPECT_FALSE(country7->getProcessedData().color);

	mappers::CultureMapper culMapper;
	politicalManager.convertAllCountries(clayManager, culMapper, {}, {}, {}, v3LocLoader, eu4LocLoader, false); // now we process only the 3 vanilla countries.

	EXPECT_FALSE(country1->getProcessedData().color); // these 3 eu4 countries still have no color.
	EXPECT_FALSE(country2->getProcessedData().color);
	EXPECT_FALSE(country3->getProcessedData().color);
	EXPECT_EQ(commonItems::Color(std::array{147, 130, 110}), country4->getProcessedData().color); // however these 3 vanilla ones have their color copied over.
	EXPECT_EQ(commonItems::Color(std::array{0.99f, 0.7f, 0.9f}), country5->getProcessedData().color); // since they are processed standalone.
	EXPECT_EQ(commonItems::Color(std::array{62, 122, 189}), country6->getProcessedData().color);
	EXPECT_EQ(commonItems::Color(std::array{62, 122, 190}), country7->getProcessedData().color);
}

TEST(V3World_PoliticalManagerTests, PoliticalManagerCanGenerateDecentralizedCountries)
{
	auto [politicalManager, popManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	// link = { eu4 = 4 vic3 = x000005 } # wasteland -> land
	// We need to manually create an empty substate for this province so that it can spawn a country.
	auto state = clayManager.getStates().at("STATE_TEST_LAND3");
	auto prov5 = state->getProvinces().at("x000005");
	auto sub5 = std::make_shared<V3::SubState>(state, V3::ProvinceMap({std::pair("x000005", prov5)}));
	state->addSubState(sub5);

	politicalManager.generateDecentralizedCountries(clayManager, popManager);

	// this link will produce a decentralized state with a single substate containing 4->x5 province.
	// name will be X01 as X00 is already taken by TA3 finding no mapping in country_mappings.txt.

	ASSERT_TRUE(politicalManager.getCountries().contains("X01"));
	const auto& x01 = politicalManager.getCountries().at("X01");
	EXPECT_FALSE(x01->getSourceCountry()); // this country has no EU4 source.

	ASSERT_EQ(1, x01->getSubStates().size());
	const auto& substate = x01->getSubStates()[0];
	EXPECT_TRUE(substate->getProvinces().contains("x000005"));
	ASSERT_TRUE(substate->getOwnerTag());
	EXPECT_EQ("X01", *substate->getOwnerTag());

	// culture is based on original population of STATE_TEST_LAND3 in vanilla installation, thus, swedish.
	EXPECT_TRUE(x01->getProcessedData().cultures.contains("swedish"));
}

TEST(V3World_PoliticalManagerTests, DecentralizedCountriesCanBeFilled)
{
	auto [politicalManager, popManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	// link = { eu4 = 4 vic3 = x000005 } # wasteland -> land
	// We need to manually create an empty substate for this province so that it can spawn a country.
	auto state = clayManager.getStates().at("STATE_TEST_LAND3");
	auto prov5 = state->getProvinces().at("x000005");
	auto sub5 = std::make_shared<V3::SubState>(state, V3::ProvinceMap({std::pair("x000005", prov5)}));
	state->addSubState(sub5);

	politicalManager.generateDecentralizedCountries(clayManager, popManager);
	politicalManager
		 .convertAllCountries(clayManager, culMapper, relMapper, cultureLoader, religionLoader, V3::LocalizationLoader(), EU4::EU4LocalizationLoader(), false);

	ASSERT_TRUE(politicalManager.getCountries().contains("X01"));
	const auto& x01 = politicalManager.getCountries().at("X01");

	// Name is "generated". Don't need locmappers for simple capitalization example.
	EXPECT_EQ("Swedish Federation", x01->getName("english"));
}

TEST(V3World_PoliticalManagerTests, MajorFormablesCanBeAltered)
{
	auto [politicalManager, popManager, culMapper, relMapper, clayManager, cultureLoader, religionLoader] = prepMappers();

	politicalManager.loadMajorFormables("TestFiles/vic3installation/game/common/country_formation/00_major_formables.txt");

	auto ger = std::make_shared<V3::Country>();
	V3::ProcessedData data;
	data.cultures = {"some_german"};
	ger->setProcessedData(data);
	politicalManager.addCountry({"GER", ger});

	politicalManager.alterMajorFormables();

	const auto& gerFormable = politicalManager.getMajorFormables().at("GER");

	EXPECT_THAT(gerFormable.getCultures(), testing::UnorderedElementsAre("north_german", "south_german", "weder_german", "some_german"));
}