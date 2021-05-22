#include "Leader/EU4LeaderID.h"
#include "gtest/gtest.h"

TEST(EU4World_EU4LeaderIDTests, primitivesDefaultToZero)
{
	std::stringstream input;
	const EU4::LeaderID id(input);

	ASSERT_EQ(0, id.getIDNum());
}

TEST(EU4World_EU4LeaderIDTests, idCanBeSet)
{
	std::stringstream input;
	input << "id = 1\n";
	const EU4::LeaderID id(input);

	ASSERT_EQ(1, id.getIDNum());
}
