#include "UnitTypeLoader/UnitTypeLoader.h"
#include "UnitTypeLoader/UnitTypeParser.h"
#include "gtest/gtest.h"

TEST(EU4World_UnitTypeLoaderTests, UnitTypesCanBeMatched)
{
	EU4::UnitTypeLoader unitTypeLoader;

	std::stringstream input;
	input << "type = cavalry\n";
	input << "maneuver = 2\n";
	unitTypeLoader.loadUnitType("some-cavalry", input);
	std::stringstream input2;
	input2 << "type = infantry\n";
	input2 << "maneuver = 4\n";
	unitTypeLoader.loadUnitType("some-infantry", input2);

	const auto unitType1 = unitTypeLoader.getUnitTypeForRegimentTypeName("some-cavalry");
	EXPECT_EQ("cavalry", unitType1->unitType);
	EXPECT_EQ(2, unitType1->strength);

	const auto unitType2 = unitTypeLoader.getUnitTypeForRegimentTypeName("some-infantry");
	EXPECT_EQ("infantry", unitType2->unitType);
	EXPECT_EQ(4, unitType2->strength);
}

TEST(EU4World_UnitTypeLoaderTests, UnitTypeMismatchReturnsNullOpt)
{
	EU4::UnitTypeLoader unitTypeLoader;

	std::stringstream input;
	input << "type = cavalry\n";
	input << "maneuver = 2\n";
	unitTypeLoader.loadUnitType("some-cavalry", input);
	std::stringstream input2;
	input2 << "type = infantry\n";
	input2 << "maneuver = 4\n";
	unitTypeLoader.loadUnitType("some-infantry", input2);

	const auto unitType = unitTypeLoader.getUnitTypeForRegimentTypeName("junk");
	EXPECT_EQ(std::nullopt, unitType);
}
