#include "MinorityPopMapper/MinorityPopMapper.h"
#include "PopManager/Pops/Pop.h"
#include "gtest/gtest.h"

TEST(Mappers_MinorityPopMapperTests, minorityReligionBlanksMajorityCulture)
{
	std::stringstream input;
	input << "minority = { religion = minorityReligion }";
	const mappers::MinorityPopMapper mapper(input);

	// Let's make a V3::Pop
	auto pop = V3::Pop("majorityCulture", "minorityReligion", "aristocrat", 1000);

	const auto success = mapper.blankMajorityFromMinority(pop);

	EXPECT_TRUE(success);
	EXPECT_TRUE(pop.getCulture().empty());
}

TEST(Mappers_MinorityPopMapperTests, minorityReligionMismatchDoesNothing)
{
	std::stringstream input;
	input << "minority = { religion = minorityReligion }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V3::Pop("majorityCulture", "majorityReligion", "aristocrat", 1000);

	const auto success = mapper.blankMajorityFromMinority(pop);

	EXPECT_FALSE(success);
	EXPECT_EQ("majorityCulture", pop.getCulture());
}

TEST(Mappers_MinorityPopMapperTests, minorityCultureBlanksMajorityReligion)
{
	std::stringstream input;
	input << "minority = { culture = minorityCulture }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V3::Pop("minorityCulture", "majorityReligion", "aristocrat", 1000);

	const auto success = mapper.blankMajorityFromMinority(pop);

	EXPECT_TRUE(success);
	EXPECT_TRUE(pop.getReligion().empty());
}

TEST(Mappers_MinorityPopMapperTests, minorityCultureMismatchDoesNothing)
{
	std::stringstream input;
	input << "minority = { culture = minorityCulture }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V3::Pop("majorityCulture", "majorityReligion", "aristocrat", 1000);

	const auto success = mapper.blankMajorityFromMinority(pop);

	EXPECT_FALSE(success);
	EXPECT_EQ("majorityReligion", pop.getReligion());
}

TEST(Mappers_MinorityPopMapperTests, fullIncomingMinorityDoesNotChangeButReturnsTrue)
{
	std::stringstream input;
	input << "minority = { religion = minorityReligion culture = minorityCulture }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V3::Pop("minorityCulture", "minorityReligion", "aristocrat", 1000);

	const auto success = mapper.blankMajorityFromMinority(pop);

	EXPECT_TRUE(success);
	EXPECT_EQ("minorityReligion", pop.getReligion());
	EXPECT_EQ("minorityCulture", pop.getCulture());
}

TEST(Mappers_MinorityPopMapperTests, fullIncomingMajorityDoesNotChange)
{
	std::stringstream input;
	input << "minority = { religion = minorityReligion culture = minorityCulture }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V3::Pop("majorityCulture", "majorityReligion", "aristocrat", 1000);

	const auto success = mapper.blankMajorityFromMinority(pop);

	EXPECT_FALSE(success);
	EXPECT_EQ("majorityReligion", pop.getReligion());
	EXPECT_EQ("majorityCulture", pop.getCulture());
}
