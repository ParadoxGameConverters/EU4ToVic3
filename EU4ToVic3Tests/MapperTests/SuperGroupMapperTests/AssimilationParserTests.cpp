#include "Mappers/SuperGroupMapper/AssimilationParser.h"
#include "gtest/gtest.h"

TEST(Mappers_AssimilationParserTests, AssimilationDefaultsToOne)
{
	std::stringstream input;
	const auto assimilation = mappers::AssimilationParser(input);

	EXPECT_EQ(1, assimilation.getAssimilation());
}

TEST(Mappers_AssimilationParserTests, AssimilationCanBeLoaded)
{
	std::stringstream input;
	input << "assimilation = 6";
	const auto assimilation = mappers::AssimilationParser(input);

	EXPECT_EQ(6, assimilation.getAssimilation());
}
