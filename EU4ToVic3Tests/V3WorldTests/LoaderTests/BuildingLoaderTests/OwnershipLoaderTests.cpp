#include "Loaders/BuildingLoader/OwnershipLoader/OwnershipLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>


TEST(V3World_OwnershipLoaderTests, OwnershipLoaderCanLoadOwnerships)
{
	V3::OwnershipLoader ownershipLoader;
	ownershipLoader.loadOwnership("TestFiles/configurables/economy/ownership.txt");

	V3::OwnershipData agriculture;
	agriculture.type = "building_manor_house";
	agriculture.weight = 1;
	agriculture.colonialFrac = 0.2;

	EXPECT_EQ(1, ownershipLoader.getOwnershipsFromBuilding("building_dye_plantation").size());
	EXPECT_EQ(agriculture, ownershipLoader.getOwnershipsFromBuilding("building_dye_plantation").at("building_manor_house"));
	EXPECT_EQ(agriculture, ownershipLoader.getOwnershipsFromBuilding("building_opium_plantation").at("building_manor_house"));
	EXPECT_EQ(agriculture, ownershipLoader.getOwnershipsFromBuilding("building_tea_plantation").at("building_manor_house"));

	V3::OwnershipData construction;
	construction.type = "national_service";
	construction.weight = 1;

	EXPECT_EQ(1, ownershipLoader.getOwnershipsFromBuilding("building_construction_sector").size());
	EXPECT_EQ(construction, ownershipLoader.getOwnershipsFromBuilding("building_construction_sector").at("national_service"));

	V3::OwnershipData extraction_local;
	extraction_local.type = "local";
	extraction_local.weight = 0.5;

	V3::OwnershipData extraction_nat;
	extraction_nat.type = "national";
	extraction_nat.weight = 0.1;

	V3::OwnershipData extraction_cap;
	extraction_cap.type = "building_financial_district";
	extraction_cap.weight = 0.3;
	extraction_cap.financialCenterFrac = 0.1;
	extraction_cap.colonialFrac = 0.05;
	extraction_cap.recognized = true;

	V3::OwnershipData extraction_ari;
	extraction_ari.type = "building_manor_house";
	extraction_ari.weight = 0.2;
	extraction_ari.financialCenterFrac = 0.0;
	extraction_ari.colonialFrac = 0.2;
	extraction_ari.recognized = false;

	EXPECT_EQ(4, ownershipLoader.getOwnershipsFromBuilding("building_iron_mine").size());
	EXPECT_EQ(4, ownershipLoader.getOwnershipsFromBuilding("building_coal_mine").size());
	EXPECT_EQ(extraction_local, ownershipLoader.getOwnershipsFromBuilding("building_iron_mine").at("local"));
	EXPECT_EQ(extraction_local, ownershipLoader.getOwnershipsFromBuilding("building_coal_mine").at("local"));
	EXPECT_EQ(extraction_nat, ownershipLoader.getOwnershipsFromBuilding("building_iron_mine").at("national"));
	EXPECT_EQ(extraction_nat, ownershipLoader.getOwnershipsFromBuilding("building_coal_mine").at("national"));
	EXPECT_EQ(extraction_cap, ownershipLoader.getOwnershipsFromBuilding("building_iron_mine").at("building_financial_district"));
	EXPECT_EQ(extraction_cap, ownershipLoader.getOwnershipsFromBuilding("building_coal_mine").at("building_financial_district"));
	EXPECT_EQ(extraction_ari, ownershipLoader.getOwnershipsFromBuilding("building_iron_mine").at("building_manor_house"));
	EXPECT_EQ(extraction_ari, ownershipLoader.getOwnershipsFromBuilding("building_coal_mine").at("building_manor_house"));
}
