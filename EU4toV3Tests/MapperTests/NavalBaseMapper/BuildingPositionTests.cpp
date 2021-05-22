#include "NavalBases/BuildingPosition.h"
#include "gtest/gtest.h"

TEST(Mappers_NavalBases_BuildingPositionTests, foundBaseIsFalse)
{
	std::stringstream input;
	const mappers::BuildingPosition mapper(input);

	ASSERT_FALSE(mapper.isFound());
}

TEST(Mappers_NavalBases_BuildingPositionTests, navalBaseCanBeFound)
{
	std::stringstream input;
	input << "building_position = {\n";
	input << "\tnaval_base = something\n";
	input << "}\n";
	const mappers::BuildingPosition mapper(input);

	ASSERT_TRUE(mapper.isFound());
}
