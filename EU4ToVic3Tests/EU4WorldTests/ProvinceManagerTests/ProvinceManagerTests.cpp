#include "ProvinceManager/ProvinceManager.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(EU4World_ProvinceManagerTests, defaultProvinceManagerIsEmpty)
{
	std::stringstream input;
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(input);

	EXPECT_TRUE(theProvinceManager.getAllProvinces().empty());
}

TEST(EU4World_ProvinceManagerTests, ProvinceManagerCanBeInput)
{
	std::stringstream input;
	input << "-1={}";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(input);

	EXPECT_TRUE(theProvinceManager.getAllProvinces().contains(1));
}

TEST(EU4World_ProvinceManagerTests, gettingNonExistentProvinceReturnsNullptr)
{
	std::stringstream input;
	input << "-1={}";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(input);

	EXPECT_EQ(nullptr, theProvinceManager.getProvince(42));
}

TEST(EU4World_ProvinceManagerTests, provinceCanBeRetrieved)
{
	std::stringstream input;
	input << "-1={}";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(input);

	EXPECT_EQ(1, theProvinceManager.getProvince(1)->getID());
}

TEST(EU4World_ProvinceManagerTests, provincesCanBeClassified)
{
	std::stringstream defaultMap;
	defaultMap << "sea_starts = { 1 }\n";
	defaultMap << "only_used_for_random = { 2 }\n";
	defaultMap << "lakes = { 3 }\n";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(defaultMap);

	std::stringstream definitionsCsv;
	definitionsCsv << "comment\n";
	definitionsCsv << "1;1;1;1;name1;x;\n"; // <- sea
	definitionsCsv << "2;2;2;2;name2;x;\n"; // <- rnw
	definitionsCsv << "3;3;3;3;name3;x;\n"; // <- lake
	definitionsCsv << "4;4;4;4;name4;x;\n"; // <- land
	definitionsCsv << "5;5;5;5;name5;x;\n"; // <- wasteland
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(definitionsCsv);

	EU4::RegionManager regionMapper;
	std::stringstream areaStream;
	areaStream << "land_area = { 4 } \n";
	areaStream << "sea_area = { 1 } ";
	std::stringstream regionStream;
	regionStream << "land_region = { areas = { land_area } }";
	regionStream << "sea_region = { areas = { sea_area } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "land_superregion = { land_region }\n";
	superRegionStream << "sea_superregion = { sea_region }";
	regionMapper.loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";
	provinceStream << "-2={}\n";
	provinceStream << "-3={}\n";
	provinceStream << "-4={}\n";
	provinceStream << "-5={}\n";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(provinceStream);

	theProvinceManager.loadDefaultMapParser(defaults);
	theProvinceManager.loadDefinitionScraper(definitions);
	theProvinceManager.classifyProvinces(regionMapper);

	EXPECT_TRUE(theProvinceManager.getProvince(1)->isSea());
	EXPECT_EQ(nullptr, theProvinceManager.getProvince(2)); // discarded provinces can't be retrieved
	EXPECT_EQ(nullptr, theProvinceManager.getProvince(3)); // discarded provinces can't be retrieved
	EXPECT_FALSE(theProvinceManager.getProvince(4)->isSea());
	EXPECT_FALSE(theProvinceManager.getProvince(5)->isSea());

	EXPECT_FALSE(theProvinceManager.isProvinceDiscarded(1));
	EXPECT_TRUE(theProvinceManager.isProvinceDiscarded(2));
	EXPECT_TRUE(theProvinceManager.isProvinceDiscarded(3));
	EXPECT_FALSE(theProvinceManager.isProvinceDiscarded(4));
	EXPECT_FALSE(theProvinceManager.isProvinceDiscarded(5));

	EXPECT_FALSE(theProvinceManager.isProvinceWasteland(1));
	EXPECT_FALSE(theProvinceManager.isProvinceWasteland(2));
	EXPECT_FALSE(theProvinceManager.isProvinceWasteland(3));
	EXPECT_FALSE(theProvinceManager.isProvinceWasteland(4));
	EXPECT_TRUE(theProvinceManager.isProvinceWasteland(5));
}

TEST(EU4World_ProvinceManagerTests, provincesCanBeDiscardedAndWarnedWhenAccessing)
{
	std::stringstream defaultMap;
	defaultMap << "sea_starts = { 1 }\n";
	defaultMap << "only_used_for_random = { 2 }\n";
	defaultMap << "lakes = { 3 }\n";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(defaultMap);

	std::stringstream definitionsCsv;
	definitionsCsv << "comment\n";
	definitionsCsv << "1;1;1;1;name1;x;\n"; // <- sea
	definitionsCsv << "2;2;2;2;name2;x;\n"; // <- rnw, to be discarded
	definitionsCsv << "3;3;3;3;name3;x;\n"; // <- lake, to be discarded
	definitionsCsv << "4;4;4;4;name4;x;\n"; // <- land
	definitionsCsv << "5;5;5;5;name5;x;\n"; // <- wasteland
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(definitionsCsv);

	EU4::RegionManager regionMapper;
	std::stringstream areaStream;
	areaStream << "land_area = { 4 } \n";
	areaStream << "sea_area = { 1 } ";
	std::stringstream regionStream;
	regionStream << "land_region = { areas = { land_area } }";
	regionStream << "sea_region = { areas = { sea_area } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "land_superregion = { land_region }\n";
	superRegionStream << "sea_superregion = { sea_region }";
	regionMapper.loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";
	provinceStream << "-2={}\n";
	provinceStream << "-3={}\n";
	provinceStream << "-4={}\n";
	provinceStream << "-5={}\n";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(provinceStream);

	theProvinceManager.loadDefaultMapParser(defaults);
	theProvinceManager.loadDefinitionScraper(definitions);
	theProvinceManager.classifyProvinces(regionMapper);

	EXPECT_FALSE(theProvinceManager.isProvinceDiscarded(1));
	EXPECT_TRUE(theProvinceManager.isProvinceDiscarded(2)); // We're discarding provinces outside political ownership sphere unless wastelands.
	EXPECT_TRUE(theProvinceManager.isProvinceDiscarded(3)); // We're discarding provinces outside political ownership sphere unless wastelands.
	EXPECT_FALSE(theProvinceManager.isProvinceDiscarded(4));
	EXPECT_FALSE(theProvinceManager.isProvinceDiscarded(5));

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	const auto& prov2 = theProvinceManager.getProvince(2);
	const auto& prov3 = theProvinceManager.getProvince(3);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Accessing EU4 province 2 that does not exist (Save/EU4 version mismatch?))"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"( [WARNING] Accessing EU4 province 3 that does not exist (Save/EU4 version mismatch?))"));
	EXPECT_EQ(nullptr, prov2);
	EXPECT_EQ(nullptr, prov3);

	std::cout.rdbuf(cout_buffer);
}

TEST(EU4World_ProvinceManagerTests, provincesClassificationThrowsForDefinitionMismatch)
{
	std::stringstream defaultMap;
	defaultMap << "sea_starts = { 1 }\n";
	defaultMap << "only_used_for_random = { 2 }\n";
	defaultMap << "lakes = { 3 }\n";
	EU4::DefaultMapParser defaults;
	defaults.loadDefaultMap(defaultMap);

	std::stringstream definitionsCsv;
	definitionsCsv << "comment\n";
	definitionsCsv << "1;1;1;1;name1;x;\n"; // <- sea
	definitionsCsv << "2;2;2;2;name2;x;\n"; // <- rnw
	definitionsCsv << "3;3;3;3;name3;x;\n"; // <- lake
	definitionsCsv << "4;4;4;4;name4;x;\n"; // <- land
	definitionsCsv << "5;5;5;5;name5;x;\n"; // <- wasteland
	EU4::DefinitionScraper definitions;
	definitions.loadDefinitions(definitionsCsv);

	EU4::RegionManager regionMapper;
	std::stringstream areaStream;
	areaStream << "land_area = { 4 } \n";
	areaStream << "sea_area = { 1 } ";
	std::stringstream regionStream;
	regionStream << "land_region = { areas = { land_area } }";
	regionStream << "sea_region = { areas = { sea_area } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "land_superregion = { land_region }\n";
	superRegionStream << "sea_superregion = { sea_region }";
	regionMapper.loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream provinceStream;
	provinceStream << "-1={}\n";
	provinceStream << "-2={}\n";
	provinceStream << "-3={}\n";
	provinceStream << "-4={}\n";
	provinceStream << "-5={}\n";
	provinceStream << "-6={}\n"; // <- rogue province
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(provinceStream);

	theProvinceManager.loadDefaultMapParser(defaults);
	theProvinceManager.loadDefinitionScraper(definitions);
	EXPECT_THROW(theProvinceManager.classifyProvinces(regionMapper), std::runtime_error);
}
