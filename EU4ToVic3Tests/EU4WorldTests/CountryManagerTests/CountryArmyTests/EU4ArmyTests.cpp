#include "CountryManager/CountryArmy/EU4Army.h"
#include "UnitTypeLoader/UnitTypeLoader.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4ArmyTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const auto army = EU4::EU4Army(input, "army");

	EXPECT_TRUE(army.getName().empty());
	EXPECT_FALSE(army.getArmyFloats());
	EXPECT_EQ(0, army.getLocation());
	EXPECT_TRUE(army.getRegiments().empty());
}

TEST(EU4World_EU4ArmyTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "name = \"1st Army\"\n";
	input << "location = 971\n";
	input << "regiment = {name = \"Penobscott's 1st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "regiment = {name = \"Penobscott's 2st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	const auto army = EU4::EU4Army(input, "army");

	EXPECT_EQ("1st Army", army.getName());
	EXPECT_FALSE(army.getArmyFloats());
	EXPECT_EQ(971, army.getLocation());
	EXPECT_EQ(2, army.getRegiments().size());
}

TEST(EU4World_EU4ArmyTests, regimentsCanBeUpdated)
{
	EU4::UnitTypeLoader unitTypeLoader;
	std::stringstream unitInput;
	unitInput << "type = infantry\nmaneuver = 2\n";
	unitTypeLoader.loadUnitType("native_indian_archer", unitInput);
	std::stringstream unitInput2;
	unitInput2 << "type = cavalry\nmaneuver = 4\n";
	unitTypeLoader.loadUnitType("native_indian_horseperson", unitInput2);

	std::stringstream input;
	input << "name = \"1st Army\"\n";
	input << "location = 971\n";
	input << "regiment = {name = \"Penobscott's 1st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "regiment = {name = \"Penobscott's 3st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "regiment = {name = \"Penobscott's 2st Regiment\" home = 971 type = \"native_indian_horseperson\"}\n";
	auto army = EU4::EU4Army(input, "army");

	army.updateRegimentTypes(unitTypeLoader);

	EXPECT_EQ(4, army.getTotalTypeStrength("infantry")); // two inf regiments worth 2 each
	EXPECT_EQ(4, army.getTotalTypeStrength("cavalry"));  // one cav regiment worth 4
}

TEST(EU4World_EU4ArmyTests, shipsCanBeResolved)
{
	EU4::UnitTypeLoader unitTypeLoader;
	std::stringstream unitInput;
	unitInput << "type = heavy_ship\nmaneuver = 60\n";
	unitTypeLoader.loadUnitType("the-manowar", unitInput);
	std::stringstream unitInput2;
	unitInput2 << "type = transport\nmaneuver = 4\n";
	unitTypeLoader.loadUnitType("the-transport", unitInput2);

	std::stringstream input;
	input << "name = \"1st Fleet\"\n";
	input << "location = 11\n";
	input << "ship = {name = \"Dulecha\" home = 1211 type = \"the-manowar\"}\n";
	input << "ship = {name = \"Koneba\" home = 1211 type = \"the-manowar\"}\n";
	input << "ship = {name = \"Dallol\" home = 1211 type = \"the-transport\"}\n";
	auto navy = EU4::EU4Army(input, "navy");

	navy.updateRegimentTypes(unitTypeLoader);

	EXPECT_EQ(120, navy.getTotalTypeStrength("heavy_ship")); // two heavy ships worth 60 each
	EXPECT_EQ(4, navy.getTotalTypeStrength("transport"));		// one transport worth 4.
}

TEST(EU4World_EU4ArmyTests, resolvingUnknownUnitTypesReplacesShipTypesWithSillytons)
{
	const EU4::UnitTypeLoader unitTypeLoader; // not loading anything.

	std::stringstream input;
	input << "name = \"1st Fleet\"\n";
	input << "location = 11\n";
	input << "ship = {name = \"Dulecha\" home = 1211 type = \"the-manowar\"}\n";
	input << "ship = {name = \"Koneba\" home = 1211 type = \"the-manowar\"}\n";
	input << "ship = {name = \"Dallol\" home = 1211 type = \"the-transport\"}\n";
	auto navy = EU4::EU4Army(input, "navy");

	navy.updateRegimentTypes(unitTypeLoader);

	EXPECT_EQ(3, navy.getTotalTypeStrength("transport")); // 3 transports worth 1 each.
}

TEST(EU4World_EU4ArmyTests, resolvingUnknownUnitTypesReplacesLandTypesWithSillytons)
{
	const EU4::UnitTypeLoader unitTypeLoader; // not loading anything.

	std::stringstream input;
	input << "name = \"1st Army\"\n";
	input << "location = 971\n";
	input << "regiment = {name = \"Penobscott's 1st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "regiment = {name = \"Penobscott's 3st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "regiment = {name = \"Penobscott's 2st Regiment\" home = 971 type = \"native_indian_horseperson\"}\n";
	auto army = EU4::EU4Army(input, "army");

	army.updateRegimentTypes(unitTypeLoader);

	EXPECT_EQ(3, army.getTotalTypeStrength("infantry")); // 3 infantry worth 1 each.
}
