#include "CultureMapper/WesternizationMapper/WesternizationMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_WesternizationMapperTests, traitsCanBePinged)
{
	mappers::WesternizationMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/westernization.txt");

	const std::set<std::string> traits = {"testtrait", "testtrait2"};

	EXPECT_EQ(10, mapper.getWesternizationForTraits(traits));
	EXPECT_EQ(5, mapper.getLiteracyForTraits(traits));
	EXPECT_EQ(5, mapper.getIndustryForTraits(traits));
}

TEST(Mappers_WesternizationMapperTests, MismatchedTraitsReturnMaxValues)
{
	mappers::WesternizationMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/westernization.txt");

	const std::set<std::string> traits = {"testtrait", "testtrait3"};

	EXPECT_EQ(10, mapper.getWesternizationForTraits(traits));
	EXPECT_EQ(5, mapper.getLiteracyForTraits(traits));
	EXPECT_EQ(8, mapper.getIndustryForTraits(traits));
}

TEST(Mappers_WesternizationMapperTests, unknownTraitsReturnZeroAndWarn)
{
	mappers::WesternizationMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/westernization.txt");

	const std::set<std::string> traits = {"unknown", "unknown2"};

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	EXPECT_EQ(0, mapper.getWesternizationForTraits(traits));
	EXPECT_EQ(0, mapper.getLiteracyForTraits(traits));
	EXPECT_EQ(0, mapper.getIndustryForTraits(traits));
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Trait unknown has no westernization.txt link!)"));
	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Trait unknown2 has no westernization.txt link!)"));
}
