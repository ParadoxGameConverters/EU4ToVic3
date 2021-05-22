#include "CultureMapper/CultureMappingRule.h"
#include "Regions/Areas.h"
#include "Regions/Regions.h"
#include "gtest/gtest.h"

TEST(Mappers_CultureMappingRuleTests, nonMatchingSourceReturnsNullopt)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "nonMatchingCulture", "", -1, "");
	ASSERT_EQ(std::nullopt, match);
}

TEST(Mappers_CultureMappingRuleTests, matchingSourceReturnsDestinationCulture)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", -1, "");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMappingRuleTests, ownerDistinguisherWrongNoMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "owner = OWN\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", -1, "NOT");
	ASSERT_EQ(std::nullopt, match);
}

TEST(Mappers_CultureMappingRuleTests, ownerDistinguisherRightMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "owner = OWN\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", -1, "OWN");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMappingRuleTests, religionDistinguisherWrongNoMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "religion = theReligion\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "nonMatchingCulture", "wrongReligion", -1, "");
	ASSERT_EQ(std::nullopt, match);
}

TEST(Mappers_CultureMappingRuleTests, religionDistinguisherRightMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "religion = theReligion\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "theReligion", -1, "");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMappingRuleTests, provinceDistinguisherWrongNoMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "provinceid = 42\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "nonMatchingCulture", "", 21, "");
	ASSERT_EQ(std::nullopt, match);
}


TEST(Mappers_CultureMappingRuleTests, provinceDistinguisherRightMatch)
{
	std::stringstream input;
	input << "\teu4 = sourceCulture\n";
	input << "\tvic2 = destinationCulture\n";
	input << "\tprovinceid = 42\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", 42, "");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMappingRuleTests, regionDistinguisherWrongNoMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "region = test_region";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = { test_region }";
	EU4::SuperRegions superRegions(superRegionsInput);
	std::stringstream regionsInput;
	regionsInput << "test_region = { areas = { test_area }\n";
	std::stringstream areasInput;
	areasInput << "test_area = { 1 2 3 }";
	EU4::Areas theAreas(areasInput);
	EU4::Regions regions(superRegions, theAreas, regionsInput);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", 42, "");
	ASSERT_EQ(std::nullopt, match);
}

TEST(Mappers_CultureMappingRuleTests, areaDistinguisherRightMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "region = test_area";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = { test_region }";
	EU4::SuperRegions superRegions(superRegionsInput);
	std::stringstream regionsInput;
	regionsInput << "test_region = { areas = { test_area }\n";
	std::stringstream areasInput;
	areasInput << "test_area = { 1 2 3 42 }";
	EU4::Areas theAreas(areasInput);
	EU4::Regions regions(superRegions, theAreas, regionsInput);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", 42, "");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMappingRuleTests, regionDistinguisherRightMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "region = test_region";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = { test_region }";
	EU4::SuperRegions superRegions(superRegionsInput);
	std::stringstream regionsInput;
	regionsInput << "test_region = { areas = { test_area }\n";
	std::stringstream areasInput;
	areasInput << "test_area = { 1 2 3 42 }";
	EU4::Areas theAreas(areasInput);
	EU4::Regions regions(superRegions, theAreas, regionsInput);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", 42, "");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMappingRuleTests, superRegionDistinguisherRightMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "region = test_superregion";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = { test_region }";
	EU4::SuperRegions superRegions(superRegionsInput);
	std::stringstream regionsInput;
	regionsInput << "test_region = { areas = { test_area }\n";
	std::stringstream areasInput;
	areasInput << "test_area = { 1 2 3 42 }";
	EU4::Areas theAreas(areasInput);
	EU4::Regions regions(superRegions, theAreas, regionsInput);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", 42, "");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMappingRuleTests, multipleSourceCulturesMatch)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "eu4 = sourceCulture2\n";
	input << "vic2 = destinationCulture\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture2", "", -1, "");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMappingRuleTests, regionalMatchFailsForRulesWithoutRegions)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = { test_region }";
	EU4::SuperRegions superRegions(superRegionsInput);
	std::stringstream regionsInput;
	regionsInput << "test_region = { areas = { test_area }\n";
	std::stringstream areasInput;
	areasInput << "test_area = { 1 2 3 42 }";
	EU4::Areas theAreas(areasInput);
	EU4::Regions regions(superRegions, theAreas, regionsInput);

	const auto& match = theMapper.cultureRegionalMatch(regions, "sourceCulture", "", 42, "");
	ASSERT_EQ(std::nullopt, match);
}

TEST(Mappers_CultureMappingRuleTests, regionalMatchFailsForRulesWithIncorrectRegions)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "region = test_area\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = { test_region }";
	EU4::SuperRegions superRegions(superRegionsInput);
	std::stringstream regionsInput;
	regionsInput << "test_region = { areas = { test_area test_area2 }\n";
	std::stringstream areasInput;
	areasInput << "test_area = { 1 2 3 }\n";
	areasInput << "test_area2 = { 42 }\n";
	EU4::Areas theAreas(areasInput);
	EU4::Regions regions(superRegions, theAreas, regionsInput);

	const auto& match = theMapper.cultureRegionalMatch(regions, "sourceCulture", "", 42, "");
	ASSERT_EQ(std::nullopt, match);
}

TEST(Mappers_CultureMappingRuleTests, regionalMatchPassesForRulesWithCorrectRegions)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "region = test_superregion\n";
	mappers::CultureMappingRule theMapper(input);

	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = { test_region }";
	EU4::SuperRegions superRegions(superRegionsInput);
	std::stringstream regionsInput;
	regionsInput << "test_region = { areas = { test_area }\n";
	std::stringstream areasInput;
	areasInput << "test_area = { 1 2 3 42 }";
	EU4::Areas theAreas(areasInput);
	EU4::Regions regions(superRegions, theAreas, regionsInput);

	const auto& match = theMapper.cultureRegionalMatch(regions, "sourceCulture", "", 42, "");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMappingRuleTests, fallbackMatchFailsForRulesWithRegionsOrReligions)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	input << "region = test_superregion\n";
	mappers::CultureMappingRule rule1(input);

	std::stringstream input2;
	input2 << "eu4 = sourceCulture\n";
	input2 << "vic2 = destinationCulture\n";
	input2 << "religion = correctReligion\n";
	input2 << "region = test_superregion\n";
	mappers::CultureMappingRule rule2(input2);

	std::stringstream input3;
	input3 << "eu4 = sourceCulture\n";
	input3 << "vic2 = destinationCulture\n";
	input3 << "religion = correctReligion\n";
	mappers::CultureMappingRule rule3(input3);
	
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = { test_region }";
	EU4::SuperRegions superRegions(superRegionsInput);
	std::stringstream regionsInput;
	regionsInput << "test_region = { areas = { test_area }\n";
	std::stringstream areasInput;
	areasInput << "test_area = { 1 2 3 42 }";
	EU4::Areas theAreas(areasInput);
	EU4::Regions regions(superRegions, theAreas, regionsInput);

	ASSERT_FALSE(rule1.cultureNonRegionalNonReligiousMatch(regions, "sourceCulture", "correctReligion", 42, ""));
	ASSERT_FALSE(rule2.cultureNonRegionalNonReligiousMatch(regions, "sourceCulture", "correctReligion", 42, ""));
	ASSERT_FALSE(rule3.cultureNonRegionalNonReligiousMatch(regions, "sourceCulture", "correctReligion", 42, ""));
}

TEST(Mappers_CultureMappingRuleTests, fallbackMatchPassesForRulesWithoutRegionsOrReligions)
{
	std::stringstream input;
	input << "eu4 = sourceCulture\n";
	input << "vic2 = destinationCulture\n";
	mappers::CultureMappingRule rule(input);

	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = { test_region }";
	EU4::SuperRegions superRegions(superRegionsInput);
	std::stringstream regionsInput;
	regionsInput << "test_region = { areas = { test_area }\n";
	std::stringstream areasInput;
	areasInput << "test_area = { 1 2 3 42 }";
	EU4::Areas theAreas(areasInput);
	EU4::Regions regions(superRegions, theAreas, regionsInput);

	ASSERT_TRUE(rule.cultureNonRegionalNonReligiousMatch(regions, "sourceCulture", "correctReligion", 42, ""));
}
