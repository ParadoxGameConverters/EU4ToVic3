#include "MinorityPops/MinorityPopMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_MinorityPopMapperTests, minorityReligionBlanksMajorityCulture)
{
	std::stringstream input;
	input << "minority = { religion = minorityReligion }";
	const mappers::MinorityPopMapper mapper(input);

	// Let's make a V2::Pop
	auto pop = V2::Pop("aristocrat", 1000, "majorityCulture", "minorityReligion");

	const auto success = mapper.blankMajorityFromMinority(pop);

	ASSERT_TRUE(success);
	ASSERT_TRUE(pop.getCulture().empty());
}

TEST(Mappers_MinorityPopMapperTests, minorityReligionMismatchDoesNothing)
{
	std::stringstream input;
	input << "minority = { religion = minorityReligion }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V2::Pop("aristocrat", 1000, "majorityCulture", "majorityReligion");

	const auto success = mapper.blankMajorityFromMinority(pop);

	ASSERT_FALSE(success);
	ASSERT_EQ("majorityCulture", pop.getCulture());
}

TEST(Mappers_MinorityPopMapperTests, minorityCultureBlanksMajorityReligion)
{
	std::stringstream input;
	input << "minority = { culture = minorityCulture }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V2::Pop("aristocrat", 1000, "minorityCulture", "majorityReligion");

	const auto success = mapper.blankMajorityFromMinority(pop);

	ASSERT_TRUE(success);
	ASSERT_TRUE(pop.getReligion().empty());
}

TEST(Mappers_MinorityPopMapperTests, minorityCultureMismatchDoesNothing)
{
	std::stringstream input;
	input << "minority = { culture = minorityCulture }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V2::Pop("aristocrat", 1000, "majorityCulture", "majorityReligion");

	const auto success = mapper.blankMajorityFromMinority(pop);

	ASSERT_FALSE(success);
	ASSERT_EQ("majorityReligion", pop.getReligion());
}

TEST(Mappers_MinorityPopMapperTests, fullIncomingMinorityDoesNotChangeButReturnsTrue)
{
	std::stringstream input;
	input << "minority = { religion = minorityReligion culture = minorityCulture }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V2::Pop("aristocrat", 1000, "minorityCulture", "minorityReligion");

	const auto success = mapper.blankMajorityFromMinority(pop);

	ASSERT_TRUE(success);
	ASSERT_EQ("minorityReligion", pop.getReligion());
	ASSERT_EQ("minorityCulture", pop.getCulture());
}

TEST(Mappers_MinorityPopMapperTests, fullIncomingMajorityDoesNotChange)
{
	std::stringstream input;
	input << "minority = { religion = minorityReligion culture = minorityCulture }";
	const mappers::MinorityPopMapper mapper(input);

	auto pop = V2::Pop("aristocrat", 1000, "majorityCulture", "majorityReligion");

	const auto success = mapper.blankMajorityFromMinority(pop);

	ASSERT_FALSE(success);
	ASSERT_EQ("majorityReligion", pop.getReligion());
	ASSERT_EQ("majorityCulture", pop.getCulture());
}
