#include "CulturalUnions/CulturalUnionMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_CulturalUnionMapperTests, nonCoreMatchReturnsEmpty)
{
	std::stringstream input;
	const mappers::CulturalUnionMapper culturalUnionMapper(input);

	ASSERT_TRUE(culturalUnionMapper.getCoresForCulture("a_culture").empty());
}

TEST(Mappers_CulturalUnionMapperTests, coreMatchReturnsCoreTags)
{
	std::stringstream input("link = { tag = TAG tag = TA2 culture = a_culture }");
	const mappers::CulturalUnionMapper theCulturalUnionMapper(input);

	ASSERT_EQ(2, theCulturalUnionMapper.getCoresForCulture("a_culture").size());
	ASSERT_TRUE(theCulturalUnionMapper.getCoresForCulture("a_culture").contains("TAG"));
	ASSERT_TRUE(theCulturalUnionMapper.getCoresForCulture("a_culture").contains("TA2"));
}

TEST(Mappers_CulturalUnionMapperTests, nonCultureMatchReturnsEmpty)
{
	std::stringstream input;
	const mappers::CulturalUnionMapper culturalUnionMapper(input);

	ASSERT_TRUE(culturalUnionMapper.getCulturesForCore("TAG").empty());
}

TEST(Mappers_CulturalUnionMapperTests, cultureMatchReturnsCoreTags)
{
	std::stringstream input;
	input << "link = { tag = TAG tag = TA1 culture = a_culture1 }";
	input << "link = { tag = TAG tag = TA2 culture = a_culture2 }";
	input << "link = { tag = TAG tag = TA3 culture = a_culture3 }";
	const mappers::CulturalUnionMapper theCulturalUnionMapper(input);

	ASSERT_EQ(1, theCulturalUnionMapper.getCulturesForCore("TA1").size());
	ASSERT_TRUE(theCulturalUnionMapper.getCulturesForCore("TA1").contains("a_culture1"));
	
	ASSERT_EQ(3, theCulturalUnionMapper.getCulturesForCore("TAG").size());
	ASSERT_TRUE(theCulturalUnionMapper.getCulturesForCore("TAG").contains("a_culture1"));
	ASSERT_TRUE(theCulturalUnionMapper.getCulturesForCore("TAG").contains("a_culture2"));
	ASSERT_TRUE(theCulturalUnionMapper.getCulturesForCore("TAG").contains("a_culture3"));
}
