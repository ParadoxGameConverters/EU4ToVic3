#include "Army/EU4Army.h"
#include "UnitTypes/UnitType.h"
#include "UnitTypes/UnitTypeMapper.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4ArmyTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const auto army = EU4::EU4Army(input, "army");

	ASSERT_TRUE(army.getName().empty());
	ASSERT_FALSE(army.getArmyFloats());
	ASSERT_EQ(0, army.getLocation());
	ASSERT_TRUE(army.getRegiments().empty());
}

TEST(EU4World_EU4ArmyTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "name = \"1st Army\"\n";
	input << "location = 971\n";
	input << "regiment = {name = \"Penobscott's 1st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "regiment = {name = \"Penobscott's 2st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	const auto army = EU4::EU4Army(input, "army");

	ASSERT_EQ("1st Army", army.getName());
	ASSERT_FALSE(army.getArmyFloats());
	ASSERT_EQ(971, army.getLocation());
	ASSERT_EQ(2, army.getRegiments().size());
}

TEST(EU4World_EU4ArmyTests, regimentsCanBeResolved)
{
	mappers::UnitTypeMapper unitTypeMapper;
	std::stringstream unitInput;
	unitInput << "type = infantry\nmaneuver = 2\n";
	unitTypeMapper.loadUnitType("native_indian_archer", unitInput);
	std::stringstream unitInput2;
	unitInput2 << "type = cavalry\nmaneuver = 4\n";
	unitTypeMapper.loadUnitType("native_indian_horseperson", unitInput2);

	std::stringstream input;
	input << "name = \"1st Army\"\n";
	input << "location = 971\n";
	input << "regiment = {name = \"Penobscott's 1st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "regiment = {name = \"Penobscott's 3st Regiment\" home = 971 type = \"native_indian_archer\"}\n";
	input << "regiment = {name = \"Penobscott's 2st Regiment\" home = 971 type = \"native_indian_horseperson\"}\n";
	auto army = EU4::EU4Army(input, "army");

	army.resolveRegimentTypes(unitTypeMapper);

	ASSERT_EQ(4, army.getTotalTypeStrength("infantry")); // two inf regiments worth 2 each
	ASSERT_EQ(4, army.getTotalTypeStrength("cavalry"));  // one cav regiment worth 4
}

TEST(EU4World_EU4ArmyTests, shipsCanBeResolved)
{
	mappers::UnitTypeMapper unitTypeMapper;
	std::stringstream unitInput;
	unitInput << "type = heavy_ship\nmaneuver = 60\n";
	unitTypeMapper.loadUnitType("the-manowar", unitInput);
	std::stringstream unitInput2;
	unitInput2 << "type = transport\nmaneuver = 4\n";
	unitTypeMapper.loadUnitType("the-transport", unitInput2);

	std::stringstream input;
	input << "name = \"1st Fleet\"\n";
	input << "location = 11\n";
	input << "ship = {name = \"Dulecha\" home = 1211 type = \"the-manowar\"}\n";
	input << "ship = {name = \"Koneba\" home = 1211 type = \"the-manowar\"}\n";
	input << "ship = {name = \"Dallol\" home = 1211 type = \"the-transport\"}\n";
	auto navy = EU4::EU4Army(input, "navy");

	navy.resolveRegimentTypes(unitTypeMapper);

	ASSERT_EQ(120, navy.getTotalTypeStrength("heavy_ship")); // two heavy ships worth 60 each
	ASSERT_EQ(24, navy.getTotalTypeStrength("transport"));	// one transport worth 24, not 4.
}

TEST(EU4World_EU4ArmyTests, resolvingUnknownUnitTypesThrowsException)
{
	const mappers::UnitTypeMapper unitTypeMapper;

	std::stringstream input;
	input << "name = \"1st Fleet\"\n";
	input << "location = 11\n";
	input << "ship = {name = \"Dulecha\" home = 1211 type = \"the-manowar\"}\n";
	input << "ship = {name = \"Koneba\" home = 1211 type = \"the-manowar\"}\n";
	input << "ship = {name = \"Dallol\" home = 1211 type = \"the-transport\"}\n";
	auto navy = EU4::EU4Army(input, "navy");

	ASSERT_THROW(navy.resolveRegimentTypes(unitTypeMapper), std::runtime_error);
}
