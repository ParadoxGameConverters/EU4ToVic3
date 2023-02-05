#include "ProvinceManager/ProvinceManager.h"
#include "RegionManager/RegionManager.h"
#include "gtest/gtest.h"

// This is a collective mapper test for Area, Region, SuperRegion and RegionManager.

TEST(Mappers_RegionMapperTests, regionMapperCanBeEnabled)
{
	// We start humble, it's a machine.
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	std::stringstream regionStream;
	std::stringstream superRegionStream;

	theMapper.loadRegions(areaStream, regionStream, superRegionStream);
	EXPECT_FALSE(theMapper.provinceIsInRegion(1, "test"));
	EXPECT_FALSE(theMapper.regionNameIsValid("test"));
	EXPECT_FALSE(theMapper.getParentAreaName(1));
	EXPECT_FALSE(theMapper.getParentRegionName(1));
	EXPECT_FALSE(theMapper.getParentSuperRegionName(1));
}

TEST(Mappers_RegionMapperTests, loadingBrokenAreaWillThrowException)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "broken_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	try
	{
		theMapper.loadRegions(areaStream, regionStream, superRegionStream);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		EXPECT_STREQ("Region's test_region area test_area does not exist!", e.what());
	}
}

TEST(Mappers_RegionMapperTests, loadingBrokenRegionWillThrowException)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "broken_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	try
	{
		theMapper.loadRegions(areaStream, regionStream, superRegionStream);
		FAIL();
	}
	catch (const std::runtime_error& e)
	{
		EXPECT_STREQ("Superregion's test_superregion region test_region does not exist!", e.what());
	}
}

TEST(Mappers_RegionMapperTests, locationServicesWork)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_TRUE(theMapper.provinceIsInRegion(3, "test_area"));
	EXPECT_TRUE(theMapper.provinceIsInRegion(2, "test_region"));
	EXPECT_TRUE(theMapper.provinceIsInRegion(1, "test_superregion"));

	EXPECT_TRUE(theMapper.provinceIsValid(1));
	EXPECT_TRUE(theMapper.provinceIsValid(2));
	EXPECT_TRUE(theMapper.provinceIsValid(3));
}

TEST(Mappers_RegionMapperTests, locationServicesCorrectlyFail)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_FALSE(theMapper.provinceIsInRegion(4, "test_area"));
	EXPECT_FALSE(theMapper.provinceIsInRegion(5, "test_region"));
	EXPECT_FALSE(theMapper.provinceIsInRegion(6, "test_superregion"));
}

TEST(Mappers_RegionMapperTests, locationServicesFailForNonsense)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_FALSE(theMapper.provinceIsInRegion(1, "nonsense"));
	EXPECT_FALSE(theMapper.provinceIsInRegion(9, "test_area"));

	EXPECT_FALSE(theMapper.provinceIsValid(9));
}

TEST(Mappers_RegionMapperTests, correctParentLocationsReported)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_EQ("test_area", *theMapper.getParentAreaName(1));
	EXPECT_EQ("test_region", *theMapper.getParentRegionName(1));
	EXPECT_EQ("test_superregion", *theMapper.getParentSuperRegionName(1));
	EXPECT_EQ("test_area2", *theMapper.getParentAreaName(5));
	EXPECT_EQ("test_region2", *theMapper.getParentRegionName(5));
	EXPECT_EQ("test_superregion2", *theMapper.getParentSuperRegionName(5));
}

TEST(Mappers_RegionMapperTests, wrongParentLocationsReturnNullopt)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_FALSE(theMapper.getParentAreaName(7));
	EXPECT_FALSE(theMapper.getParentRegionName(7));
	EXPECT_FALSE(theMapper.getParentSuperRegionName(7));
}

TEST(Mappers_RegionMapperTests, locationNameValidationWorks)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 2 3 } \n";
	areaStream << "test_area2 = { 4 5 6 } ";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	superRegionStream << "test_superregion2 = { test_region2 }";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	EXPECT_TRUE(theMapper.regionNameIsValid("test_area"));
	EXPECT_TRUE(theMapper.regionNameIsValid("test_region2"));
	EXPECT_TRUE(theMapper.regionNameIsValid("test_superregion2"));
	EXPECT_FALSE(theMapper.regionNameIsValid("nonsense"));
}

TEST(Mappers_RegionMapperTests, assimilationCanBeAssignedAndPinged)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 }";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream superGroupStream;
	superGroupStream << "test_supergroup = { test_superregion = {assimilation = 2}}\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(superGroupStream);

	theMapper.loadSuperGroups(superGroupMapper);
	theMapper.applySuperGroups();

	EXPECT_NEAR(0.004375, *theMapper.getAssimilationFactor(1), 0.0001);
}

TEST(Mappers_RegionMapperTests, assimilationMismatchReturnsNullopt)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 }";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream superGroupStream;
	superGroupStream << "test_supergroup = { test_superregion = {assimilation = 2}}\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(superGroupStream);

	theMapper.loadSuperGroups(superGroupMapper);
	theMapper.applySuperGroups();

	EXPECT_EQ(std::nullopt, theMapper.getAssimilationFactor(99));
}

TEST(Mappers_RegionMapperTests, superGroupsCanBeAssignedAndPinged)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 }";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream superGroupStream;
	superGroupStream << "test_supergroup = { test_superregion = {assimilation = 2}}\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(superGroupStream);

	theMapper.loadSuperGroups(superGroupMapper);
	theMapper.applySuperGroups();

	EXPECT_EQ("test_supergroup", theMapper.getParentSuperGroupName(1));
}

TEST(Mappers_RegionMapperTests, supergroupMismatchReturnsNullopt)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 }";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream superGroupStream;
	superGroupStream << "test_supergroup = { test_superregion = {assimilation = 2}}\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(superGroupStream);

	theMapper.loadSuperGroups(superGroupMapper);
	theMapper.applySuperGroups();

	EXPECT_EQ(std::nullopt, theMapper.getParentSuperGroupName(99));
}

TEST(Mappers_RegionMapperTests, brokenSupergroupDefaultsToOldWorld)
{
	EU4::RegionManager theMapper;
	std::stringstream areaStream;
	areaStream << "test_area = { 1 }";
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion = { test_region }\n";
	theMapper.loadRegions(areaStream, regionStream, superRegionStream);

	std::stringstream superGroupStream;
	superGroupStream << "test_supergroup = { test_superregionBROKE = {assimilation = 2}}\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(superGroupStream);

	theMapper.loadSuperGroups(superGroupMapper);
	theMapper.applySuperGroups();

	EXPECT_EQ("old_world", theMapper.getParentSuperGroupName(1));
}

TEST(Mappers_RegionMapperTests, colonialRegionForProvinceCanBeRetrieved)
{
	std::stringstream colonialInput;
	colonialInput << "colonial_alaska = {\n";
	colonialInput << "	provinces = {\n";
	colonialInput << "		1 2 3\n";
	colonialInput << "	}\n";
	colonialInput << "}\n";
	EU4::ColonialRegionLoader regions;
	regions.loadColonialRegions(colonialInput);

	EU4::RegionManager mapper;
	mapper.loadColonialRegions(regions);

	EXPECT_EQ("colonial_alaska", mapper.getColonialRegionForProvince(1));
	EXPECT_EQ("colonial_alaska", mapper.getColonialRegionForProvince(2));
	EXPECT_EQ("colonial_alaska", mapper.getColonialRegionForProvince(3));
	EXPECT_EQ(std::nullopt, mapper.getColonialRegionForProvince(4));
}

TEST(Mappers_RegionMapperTests, nativeCulturesCanBeCataloguedAndBrowsed)
{
	std::stringstream worldInput;
	worldInput << "old_world = { test_superregion3 = {assimilation = 2} }\n";
	worldInput << "new_world = { test_superregion1 = {assimilation = 2} test_superregion2 = {assimilation = 2} }\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(worldInput);

	std::stringstream colonialInput;
	colonialInput << "colonial_alaska = {\n";
	colonialInput << "	provinces = {\n";
	colonialInput << "		1\n";
	colonialInput << "	}\n";
	colonialInput << "}\n";
	EU4::ColonialRegionLoader regions;
	regions.loadColonialRegions(colonialInput);

	EU4::RegionManager mapper;
	mapper.loadColonialRegions(regions);

	EXPECT_EQ("colonial_alaska", mapper.getColonialRegionForProvince(1));
	EXPECT_EQ(std::nullopt, mapper.getColonialRegionForProvince(2));

	std::stringstream areaStream;
	areaStream << "test_area = { 1 } \n"; // <- in colonial region
	areaStream << "test_area2 = { 2 } ";  // <- not colonial but same continent/superregion/supergroup
	areaStream << "test_area3 = { 3 } ";  // old world
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	regionStream << "test_region3 = { areas = { test_area3 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion1 = { test_region }\n"; // <- colonial superregion, new world
	superRegionStream << "test_superregion2 = { test_region2 }";  // new world
	superRegionStream << "test_superregion3 = { test_region3 }";  // old world
	mapper.loadRegions(areaStream, regionStream, superRegionStream);
	mapper.loadSuperGroups(superGroupMapper);
	mapper.applySuperGroups();

	std::stringstream provincesInput;
	provincesInput << "-1={ history = { culture = nativeCulture } }\n";
	provincesInput << "-2={ history = { culture = otherNativeCulture } }\n}";
	provincesInput << "-3={ history = { culture = invasiveCulture } }\n";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(provincesInput);

	EXPECT_EQ("nativeCulture", theProvinceManager.getProvince(1)->getStartingCulture());
	EXPECT_EQ("otherNativeCulture", theProvinceManager.getProvince(2)->getStartingCulture());
	EXPECT_EQ("invasiveCulture", theProvinceManager.getProvince(3)->getStartingCulture());

	mapper.catalogueNativeCultures(theProvinceManager);

	EXPECT_FALSE(mapper.doesProvinceRequireNeoCulture(1, "nativeCulture"));		  // false because it's native to the region
	EXPECT_FALSE(mapper.doesProvinceRequireNeoCulture(1, "otherNativeCulture")); // false because it's native to the new world
	EXPECT_TRUE(mapper.doesProvinceRequireNeoCulture(1, "invasiveCulture"));	  // true as not native and province is colonial

	EXPECT_FALSE(mapper.doesProvinceRequireNeoCulture(2, "nativeCulture"));		  // false because not colonial
	EXPECT_FALSE(mapper.doesProvinceRequireNeoCulture(2, "otherNativeCulture")); // false because not colonial
	EXPECT_FALSE(mapper.doesProvinceRequireNeoCulture(2, "invasiveCulture"));	  // false because not colonial

	EXPECT_FALSE(mapper.doesProvinceRequireNeoCulture(3, "nativeCulture"));		  // false because not colonial
	EXPECT_FALSE(mapper.doesProvinceRequireNeoCulture(3, "otherNativeCulture")); // false because not colonial
	EXPECT_FALSE(mapper.doesProvinceRequireNeoCulture(3, "invasiveCulture"));	  // false because not colonial
}

TEST(Mappers_RegionMapperTests, invasiveCulturesCanBeFlagged)
{
	std::stringstream worldInput;
	worldInput << "old_world = { test_superregion3 = {assimilation = 2} }\n";
	worldInput << "new_world = { test_superregion1 = {assimilation = 2} test_superregion2 = {assimilation = 2} }\n";
	mappers::SuperGroupMapper superGroupMapper;
	superGroupMapper.loadSuperGroups(worldInput);

	std::stringstream colonialInput;
	colonialInput << "colonial_alaska = {\n";
	colonialInput << "	provinces = {\n";
	colonialInput << "		1\n";
	colonialInput << "	}\n";
	colonialInput << "}\n";
	EU4::ColonialRegionLoader regions;
	regions.loadColonialRegions(colonialInput);

	EU4::RegionManager mapper;
	mapper.loadColonialRegions(regions);

	EXPECT_EQ("colonial_alaska", mapper.getColonialRegionForProvince(1));
	EXPECT_EQ(std::nullopt, mapper.getColonialRegionForProvince(2));

	std::stringstream areaStream;
	areaStream << "test_area = { 1 } \n"; // <- in colonial region
	areaStream << "test_area2 = { 2 } ";  // <- not colonial but same continent/superregion/supergroup
	areaStream << "test_area3 = { 3 } ";  // old world
	std::stringstream regionStream;
	regionStream << "test_region = { areas = { test_area } }";
	regionStream << "test_region2 = { areas = { test_area2 } }\n";
	regionStream << "test_region3 = { areas = { test_area3 } }\n";
	std::stringstream superRegionStream;
	superRegionStream << "test_superregion1 = { test_region }\n"; // <- colonial superregion, new world
	superRegionStream << "test_superregion2 = { test_region2 }";  // new world
	superRegionStream << "test_superregion3 = { test_region3 }";  // old world
	mapper.loadRegions(areaStream, regionStream, superRegionStream);
	mapper.loadSuperGroups(superGroupMapper);
	mapper.applySuperGroups();

	std::stringstream provincesInput;
	provincesInput << "-1={ history = { culture = nativeCulture religion = rel } }\n";
	provincesInput << "-2={ history = { culture = otherNativeCulture religion = rel } }\n}";
	provincesInput << "-3={ history = { culture = invasiveCulture religion = rel } }\n";
	EU4::ProvinceManager theProvinceManager;
	theProvinceManager.loadProvinces(provincesInput);

	EXPECT_EQ("nativeCulture", theProvinceManager.getProvince(1)->getStartingCulture());
	EXPECT_EQ("otherNativeCulture", theProvinceManager.getProvince(2)->getStartingCulture());
	EXPECT_EQ("invasiveCulture", theProvinceManager.getProvince(3)->getStartingCulture());

	mapper.catalogueNativeCultures(theProvinceManager);

	// let's put all cultures in province 1 which generates neocultures.
	const auto& prov1 = theProvinceManager.getProvince(1);
	prov1->buildPopRatios(DatingData()); // build native ratio
	auto history = prov1->getProvinceHistory();
	// first ratio is there:
	EXPECT_EQ(1, history.getPopRatios().size());
	history.addPopRatio(EU4::PopRatio("otherNativeCulture", "")); // add the other 2.
	history.addPopRatio(EU4::PopRatio("invasiveCulture", ""));
	prov1->setProvinceHistory(history);

	mapper.flagNeoCultures(theProvinceManager);

	// now grab all 3 popratios.
	const auto& pop1 = prov1->getProvinceHistory().getPopRatios()[0];
	const auto& pop2 = prov1->getProvinceHistory().getPopRatios()[1];
	const auto& pop3 = prov1->getProvinceHistory().getPopRatios()[2];

	EXPECT_EQ("nativeCulture", pop1.getCulture());
	EXPECT_FALSE(pop1.isNeoCulture());

	EXPECT_EQ("otherNativeCulture", pop2.getCulture());
	EXPECT_FALSE(pop2.isNeoCulture());

	EXPECT_EQ("invasiveCulture", pop3.getCulture());
	EXPECT_TRUE(pop3.isNeoCulture());
}
