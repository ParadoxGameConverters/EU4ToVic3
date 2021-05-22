#include "FactoryCounts/FactoryStartingCounts.h"
#include "gtest/gtest.h"

TEST(Mappers_FactoryCountsTests, countsCanBePinged)
{
	std::stringstream input;
	input << "factory1 = 3\n";
	input << "factory2 = 5\n";
	const mappers::FactoryStartingCounts mapper(input);

	ASSERT_EQ(3, *mapper.getCountForFactoryType("factory1"));
	ASSERT_EQ(5, *mapper.getCountForFactoryType("factory2"));
}

TEST(Mappers_FactoryCountsTests, mismatchReturnsNullopt)
{
	std::stringstream input;
	input << "factory1 = 3\n";
	input << "factory2 = 5\n";
	const mappers::FactoryStartingCounts mapper(input);

	ASSERT_EQ(std::nullopt, mapper.getCountForFactoryType("factory-error"));
}
