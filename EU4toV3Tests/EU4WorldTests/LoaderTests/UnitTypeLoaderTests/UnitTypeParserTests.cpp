#include "UnitTypeLoader/UnitTypeParser.h"
#include "gtest/gtest.h"

TEST(EU4world_UnitTypeParserTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const EU4::UnitTypeParser unitType(input);

	EXPECT_EQ(0, unitType.getUnitType().strength);
	EXPECT_TRUE(unitType.getUnitType().unitType.empty());
}

TEST(EU4world_UnitTypeParserTests, regimentLoadsPrimitives)
{
	std::stringstream input;
	input << "type = cavalry\n";
	input << "unit_type = sub_saharan\n"; // irrelevant
	input << "maneuver = 2\n";
	input << "offensive_morale = 3\n";
	input << "defensive_morale = 3\n";
	input << "offensive_fire = 0\n";
	input << "defensive_fire = 0\n";
	input << "offensive_shock = 2\n";
	input << "defensive_shock = 2\n";
	const EU4::UnitTypeParser unitType(input);

	EXPECT_EQ(12, unitType.getUnitType().strength); // Sum of all pips loaded.
	EXPECT_EQ("cavalry", unitType.getUnitType().unitType);
}
