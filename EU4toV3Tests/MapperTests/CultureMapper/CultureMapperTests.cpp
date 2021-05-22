#include "CultureMapper/CultureMapper.h"
#include "Regions/Areas.h"
#include "Regions/Regions.h"
#include "gtest/gtest.h"

TEST(Mappers_CultureMapperTests, nonMatchReturnsNullopt)
{
	std::stringstream input;
	input << "link = { eu4 = sourceCulture vic2 = destinationCulture }\n";
	mappers::CultureMapper theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "nonMatchingCulture", "", -1, "");
	ASSERT_EQ(std::nullopt, match);
}

TEST(Mappers_CultureMapperTests, matchReturnsCulture)
{
	std::stringstream input;
	input << "link = { eu4 = sourceCulture vic2 = destinationCulture }\n";
	mappers::CultureMapper theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", -1, "");
	ASSERT_EQ("destinationCulture", *match);
}

TEST(Mappers_CultureMapperTests, firstApplicableRuleMatches)
{
	std::stringstream input;
	input << "link = { eu4 = sourceCulture vic2 = destinationCulture1 }\n";
	input << "link = { eu4 = sourceCulture vic2 = destinationCulture2 }\n";
	mappers::CultureMapper theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "", -1, "");
	ASSERT_EQ("destinationCulture1", *match);
}

TEST(Mappers_CultureMapperTests, correctRuleMaches)
{
	std::stringstream input;
	input << "link = { eu4 = sourceCulture vic2 = destinationCulture1 religion = correctReligion}\n";
	input << "link = { eu4 = sourceCulture vic2 = destinationCulture2 }\n";
	mappers::CultureMapper theMapper(input);

	std::stringstream areasInput;
	EU4::Areas areas(areasInput);
	EU4::Regions regions(areas);

	const auto& match = theMapper.cultureMatch(regions, "sourceCulture", "wrongReligion", -1, "");
	ASSERT_EQ("destinationCulture2", *match);
}
