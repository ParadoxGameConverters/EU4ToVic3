#include "CountryMappings/CountryMapping.h"
#include "gtest/gtest.h"

TEST(Mappers_CountryMappingTests, SourceCountryDefaultsToBlank)
{
	std::stringstream input;
	const mappers::CountryMapping countryMapping(input);

	ASSERT_TRUE(countryMapping.getEU4Tag().empty());
}

TEST(Mappers_CountryMappingTests, SourceCountryCanBeSet)
{
	std::stringstream input;
	input << "eu4 = TAG";
	const mappers::CountryMapping countryMapping(input);

	ASSERT_EQ("TAG", countryMapping.getEU4Tag());
}

TEST(Mappers_CountryMappingTests, TargetCountryDefaultstoBlank)
{
	std::stringstream input;
	const mappers::CountryMapping countryMapping(input);

	ASSERT_TRUE(countryMapping.getVic2Tag().empty());
}

TEST(Mappers_CountryMappingTests, TargetCountryCanBeSet)
{
	std::stringstream input;
	input << "v2 = TAG";
	const mappers::CountryMapping countryMapping(input);

	ASSERT_EQ("TAG", countryMapping.getVic2Tag());
}

TEST(Mappers_CountryMappingTests, FlagsDefaultToEmpty)
{
	std::stringstream input;
	const mappers::CountryMapping countryMapping(input);

	ASSERT_TRUE(countryMapping.getFlags().empty());
}

TEST(Mappers_CountryMappingTests, FlagsCanBeLoaded)
{
	std::stringstream input;
	input << "flag = flag1";
	input << "\tflag = flag1";
	input << "\tflag = flag2";
	input << "\tflag = flag1";
	const mappers::CountryMapping countryMapping(input);

	ASSERT_EQ(2, countryMapping.getFlags().size());
	ASSERT_TRUE(countryMapping.getFlags().contains("flag1"));
	ASSERT_TRUE(countryMapping.getFlags().contains("flag2"));
}

TEST(Mappers_CountryMappingTests, ReformsDefaultToEmpty)
{
	std::stringstream input;
	const mappers::CountryMapping countryMapping(input);

	ASSERT_TRUE(countryMapping.getReforms().empty());
}

TEST(Mappers_CountryMappingTests, ReformsCanBeLoaded)
{
	std::stringstream input;
	input << "reform = reform1";
	input << "\treform = reform1";
	input << "\treform = reform2";
	input << "\treform = reform1";
	const mappers::CountryMapping countryMapping(input);

	ASSERT_EQ(2, countryMapping.getReforms().size());
	ASSERT_TRUE(countryMapping.getReforms().contains("reform1"));
	ASSERT_TRUE(countryMapping.getReforms().contains("reform2"));
}
