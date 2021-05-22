#include "Configuration.h"
#include "Country/Countries.h"
#include "CountryMappings/CountryMappings.h"
#include "CultureGroups/CultureGroups.h"
#include "ProvinceMappings/ProvinceMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_CountryMappingsTests, trivialLinksCanBeMapped)
{
	std::stringstream titleMapperStream;
	std::stringstream v2RegionsStream;
	std::stringstream colonialTagStream;
	std::stringstream mainInput;
	mainInput << "link = { eu4 = SWE v2 = DEN }";
	mappers::CountryMappings mapper(mainInput, titleMapperStream, v2RegionsStream, colonialTagStream);

	std::stringstream ieInput;
	mappers::IdeaEffectMapper ideaEffectMapper(ieInput);
	GameVersion version(0, 0, 0, 0);
	std::stringstream countryInput;
	countryInput << "SWE = {}";
	EU4::Countries countries(version, countryInput, ideaEffectMapper);

	const Configuration testConfiguration;
	std::stringstream colonialInput;
	std::stringstream provinceInput;
	provinceInput << "0.0.0.0 = {}";
	mappers::ProvinceMapper provinceMapper(provinceInput, colonialInput, testConfiguration);

	std::stringstream cgInput;
	auto cultureGroupsMapper = std::make_shared<mappers::CultureGroups>(cgInput);
	mapper.createMappings(cultureGroupsMapper, countries.getTheCountries(), provinceMapper);

	// Trivial, right?

	const auto& match = mapper.getV2Tag("SWE");

	ASSERT_EQ("DEN", match);
}

TEST(Mappers_CountryMappingsTests, titleNameGuesstimatesCanBeMapped)
{
	std::stringstream v2RegionsStream;
	std::stringstream colonialTagStream;
	std::stringstream titleMapperStream;
	titleMapperStream << "link = { name = \"sweden\" title = k_sweden region = e_scandinavia }";
	std::stringstream mainInput;
	mainInput << "link = { eu4 = k_sweden v2 = DEN }";
	mappers::CountryMappings mapper(mainInput, titleMapperStream, v2RegionsStream, colonialTagStream);

	std::stringstream ieInput;
	mappers::IdeaEffectMapper ideaEffectMapper(ieInput);
	GameVersion version(0, 0, 0, 0);
	std::stringstream countryInput;
	countryInput << "X89 = { name = \"Sweden\"}";
	EU4::Countries countries(version, countryInput, ideaEffectMapper);

	const Configuration testConfiguration;
	std::stringstream colonialInput;
	std::stringstream provinceInput;
	provinceInput << "0.0.0.0 = {}";
	mappers::ProvinceMapper provinceMapper(provinceInput, colonialInput, testConfiguration);

	std::stringstream cgInput;
	auto cultureGroupsMapper = std::make_shared<mappers::CultureGroups>(cgInput);

	mapper.createMappings(cultureGroupsMapper, countries.getTheCountries(), provinceMapper);

	const auto& match = mapper.getV2Tag("X89");

	ASSERT_EQ("DEN", match);
}

TEST(Mappers_CountryMappingsTests, unmatchedLinksMapToDynamicTags)
{
	std::stringstream titleMapperStream;
	std::stringstream v2RegionsStream;
	std::stringstream colonialTagStream;
	std::stringstream mainInput;
	mappers::CountryMappings mapper(mainInput, titleMapperStream, v2RegionsStream, colonialTagStream);

	GameVersion version(0, 0, 0, 0);
	std::stringstream ieInput;
	mappers::IdeaEffectMapper ideaEffectMapper(ieInput);
	std::stringstream countryInput;
	countryInput << "X89 = {}";
	EU4::Countries countries(version, countryInput, ideaEffectMapper);

	const Configuration testConfiguration;
	std::stringstream colonialInput;
	std::stringstream provinceInput;
	provinceInput << "0.0.0.0 = {}";
	mappers::ProvinceMapper provinceMapper(provinceInput, colonialInput, testConfiguration);

	std::stringstream cgInput;
	auto cultureGroupsMapper = std::make_shared<mappers::CultureGroups>(cgInput);
	mapper.createMappings(cultureGroupsMapper, countries.getTheCountries(), provinceMapper);

	const auto& match = mapper.getV2Tag("X89");

	ASSERT_EQ("X00", *match);
}

TEST(Mappers_CountryMappingsTests, colonialReplacementsCanBeDefined)
{
	std::stringstream titleMapperStream;
	std::stringstream v2RegionsStream;
	v2RegionsStream << "ENG_30 = { 100 }";
	std::stringstream colonialTagStream;
	colonialTagStream << "link = { tag = UNY eu4region = colonial_eastern_america v2region = ENG_30 cultureGroup = british }";
	std::stringstream mainInput;
	mappers::CountryMappings mapper(mainInput, titleMapperStream, v2RegionsStream, colonialTagStream);

	GameVersion version(0, 0, 0, 0);
	std::stringstream ieInput;
	mappers::IdeaEffectMapper ideaEffectMapper(ieInput);
	std::stringstream countryInput;
	countryInput << "C01 = { name = \"New Lancaster\" primary_culture = english capital = 1 colonial_parent = ENG }";
	EU4::Countries countries(version, countryInput, ideaEffectMapper);

	const Configuration testConfiguration;
	std::stringstream colonialInput;
	colonialInput << "colonial_eastern_america = { provinces = { 1 } }";
	std::stringstream provinceInput;
	provinceInput << "0.0.0.0 = { link = { eu4 = 1 v2 = 100 }";
	mappers::ProvinceMapper provinceMapper(provinceInput, colonialInput, testConfiguration);

	std::stringstream cgInput;
	cgInput << "british = { english }";
	auto cultureGroupsMapper = std::make_shared<mappers::CultureGroups>(cgInput);
	mapper.createMappings(cultureGroupsMapper, countries.getTheCountries(), provinceMapper);

	const auto& match = mapper.getV2Tag("C01");

	ASSERT_EQ("UNY", *match);
}

TEST(Mappers_CountryMappingsTests, V2GetterReturnsTagForMappedTags)
{
	std::stringstream titleMapperStream;
	std::stringstream v2RegionsStream;
	std::stringstream colonialTagStream;
	std::stringstream mainInput;
	mainInput << "link = { eu4 = SWE v2 = DEN }";
	mappers::CountryMappings mapper(mainInput, titleMapperStream, v2RegionsStream, colonialTagStream);

	GameVersion version(0, 0, 0, 0);
	std::stringstream ieInput;
	mappers::IdeaEffectMapper ideaEffectMapper(ieInput);
	std::stringstream countryInput;
	countryInput << "SWE = {}";
	EU4::Countries countries(version, countryInput, ideaEffectMapper);

	const Configuration testConfiguration;
	std::stringstream colonialInput;
	std::stringstream provinceInput;
	provinceInput << "0.0.0.0 = {}";
	mappers::ProvinceMapper provinceMapper(provinceInput, colonialInput, testConfiguration);

	std::stringstream cgInput;
	auto cultureGroupsMapper = std::make_shared<mappers::CultureGroups>(cgInput);
	mapper.createMappings(cultureGroupsMapper, countries.getTheCountries(), provinceMapper);

	ASSERT_EQ("DEN", *mapper.getV2Tag("SWE"));
}

TEST(Mappers_CountryMappingsTests, V2GetterReturnsNulloptForUnmappedTags)
{
	std::stringstream titleMapperStream;
	std::stringstream v2RegionsStream;
	std::stringstream colonialTagStream;
	std::stringstream mainInput;
	mainInput << "link = { eu4 = SWE v2 = DEN }";
	mappers::CountryMappings mapper(mainInput, titleMapperStream, v2RegionsStream, colonialTagStream);

	GameVersion version(0, 0, 0, 0);
	std::stringstream ieInput;
	mappers::IdeaEffectMapper ideaEffectMapper(ieInput);
	std::stringstream countryInput;
	countryInput << "SWE = {}";
	EU4::Countries countries(version, countryInput, ideaEffectMapper);

	const Configuration testConfiguration;
	std::stringstream colonialInput;
	std::stringstream provinceInput;
	provinceInput << "0.0.0.0 = {}";
	mappers::ProvinceMapper provinceMapper(provinceInput, colonialInput, testConfiguration);

	std::stringstream cgInput;
	auto cultureGroupsMapper = std::make_shared<mappers::CultureGroups>(cgInput);
	mapper.createMappings(cultureGroupsMapper, countries.getTheCountries(), provinceMapper);

	ASSERT_EQ(std::nullopt, mapper.getV2Tag("DEN"));
}

TEST(Mappers_CountryMappingsTests, getTitleReturnsTitleGuesstimate)
{
	std::stringstream titleMapperStream;
	titleMapperStream << "link = { name = \"sweden\" title = k_sverige region = e_scandinavia }";
	std::stringstream v2RegionsStream;
	std::stringstream colonialTagStream;
	std::stringstream mainInput;
	mainInput << "link = { eu4 = k_sweden v2 = DEN }";
	mappers::CountryMappings mapper(mainInput, titleMapperStream, v2RegionsStream, colonialTagStream);

	ASSERT_EQ("k_sverige", *mapper.getTitle("SWEDEN"));
}

TEST(Mappers_CountryMappingsTests, getTitleCanTapInTheDark)
{
	std::stringstream titleMapperStream;
	titleMapperStream << "link = { name = \"svebjod\" title = k_sweden region = e_scandinavia }";
	std::stringstream v2RegionsStream;
	std::stringstream colonialTagStream;
	std::stringstream mainInput;
	mainInput << "link = { eu4 = k_sweden v2 = DEN }";
	mappers::CountryMappings mapper(mainInput, titleMapperStream, v2RegionsStream, colonialTagStream);

	ASSERT_EQ("k_sweden", *mapper.getTitle("Sweden"));
}

TEST(Mappers_CountryMappingsTests, getTitleReturnsNulloptOnAbjectFailure)
{
	std::stringstream titleMapperStream;
	titleMapperStream << "link = { name = \"svebjod\" title = k_sverige region = e_scandinavia }";
	std::stringstream v2RegionsStream;
	std::stringstream colonialTagStream;
	std::stringstream mainInput;
	mainInput << "link = { eu4 = k_sweden v2 = DEN }";
	mappers::CountryMappings mapper(mainInput, titleMapperStream, v2RegionsStream, colonialTagStream);

	ASSERT_EQ(std::nullopt, mapper.getTitle("Sweden"));
}

TEST(Mappers_CountryMappingsTests, tagIsAlphaDigitDigitWorksAsAdvertised)
{
	ASSERT_TRUE(mappers::CountryMappings::tagIsAlphaDigitDigit("C01"));
	ASSERT_FALSE(mappers::CountryMappings::tagIsAlphaDigitDigit("CC1"));
	ASSERT_FALSE(mappers::CountryMappings::tagIsAlphaDigitDigit("CCC"));
	ASSERT_FALSE(mappers::CountryMappings::tagIsAlphaDigitDigit("0C0"));
	ASSERT_FALSE(mappers::CountryMappings::tagIsAlphaDigitDigit("0CC"));
	ASSERT_FALSE(mappers::CountryMappings::tagIsAlphaDigitDigit("00C"));
}
