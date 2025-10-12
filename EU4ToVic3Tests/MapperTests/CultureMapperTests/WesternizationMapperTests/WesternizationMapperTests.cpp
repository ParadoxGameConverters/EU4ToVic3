#include "CultureMapper/CultureDefinitionLoader/CultureDef.h"
#include "CultureMapper/WesternizationMapper/WesternizationMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_WesternizationMapperTests, traitsCanBePinged)
{
	mappers::WesternizationMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/westernization.txt");

	mappers::CultureDef culture;
	culture.language = "testtrait";
	culture.heritage = "testtrait2";

	EXPECT_EQ(10, mapper.getWesternizationForCulture(culture));
	EXPECT_EQ(5, mapper.getLiteracyForCulture(culture));
	EXPECT_EQ(5, mapper.getIndustryForCulture(culture));
}

TEST(Mappers_WesternizationMapperTests, MismatchedTraitsReturnMaxValues)
{
	mappers::WesternizationMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/westernization.txt");

	mappers::CultureDef culture;
	culture.language = "testtrait";
	culture.heritage = "testtrait3";

	EXPECT_EQ(10, mapper.getWesternizationForCulture(culture));
	EXPECT_EQ(5, mapper.getLiteracyForCulture(culture));
	EXPECT_EQ(8, mapper.getIndustryForCulture(culture));
}

TEST(Mappers_WesternizationMapperTests, unknownTraitsReturnZeroAndWarn)
{
	mappers::WesternizationMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/westernization.txt");

	mappers::CultureDef culture;
	culture.name = "random";
	culture.language = "unknown";
	culture.heritage = "unknown2";

	std::stringstream log;
	std::streambuf* cout_buffer = std::cout.rdbuf();
	std::cout.rdbuf(log.rdbuf());
	EXPECT_EQ(0, mapper.getWesternizationForCulture(culture));
	EXPECT_EQ(0, mapper.getLiteracyForCulture(culture));
	EXPECT_EQ(0, mapper.getIndustryForCulture(culture));
	std::cout.rdbuf(cout_buffer);

	EXPECT_THAT(log.str(), testing::HasSubstr(R"([WARNING] Culture random has no westernization.txt traits linked!)"));
}
