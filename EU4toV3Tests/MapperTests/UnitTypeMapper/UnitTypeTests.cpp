#include "UnitTypes/UnitType.h"
#include "gtest/gtest.h"

TEST(Mappers_UnitTypeTests, primitivesDefaultToBlank)
{
	std::stringstream input;
	const mappers::UnitType unitType(input);

	ASSERT_EQ(0, unitType.getStrength());
	ASSERT_TRUE(unitType.getCategory().empty());
}

TEST(Mappers_UnitTypeTests, regimentLoadsPrimitives)
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

	const mappers::UnitType unitType(input);

	ASSERT_EQ(12, unitType.getStrength()); // Sum of all pips loaded.
	ASSERT_EQ("cavalry", unitType.getCategory());
}
