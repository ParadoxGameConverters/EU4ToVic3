#include "ProvinceManager/ProvinceManager.h"
#include "gtest/gtest.h"

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

TEST(EU4World_ProvinceManagerTests, gettingNonExistentProvinceThrowsException)
{
	std::stringstream input;
	input << "-1={}";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(input);

	EXPECT_THROW(auto breakPlease = theProvinceManager.getProvince(42), std::runtime_error);
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
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(provinceStream);

	theProvinceManager.loadParsers(defaults, definitions);
	theProvinceManager.classifyProvinces(regionMapper);

	EXPECT_TRUE(theProvinceManager.getProvince(1)->isSea());
	EXPECT_THROW(auto fail = theProvinceManager.getProvince(2), std::runtime_error);
	EXPECT_THROW(auto fail = theProvinceManager.getProvince(3), std::runtime_error);
	EXPECT_FALSE(theProvinceManager.getProvince(4)->isSea());
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
	provinceStream << "-5={}\n"; // <- rogue province
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(provinceStream);

	theProvinceManager.loadParsers(defaults, definitions);
	EXPECT_THROW(theProvinceManager.classifyProvinces(regionMapper), std::runtime_error);
}
