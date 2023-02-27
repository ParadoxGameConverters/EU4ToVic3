#include "CountryManager/EU4Country.h"
#include "CountryMapper/CountryMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_CountryMapperTests, rulesCanBeLoadedInOrder)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");

	EXPECT_EQ(13, mapper.getMappingRules().size());
	const auto& rule1 = mapper.getMappingRules()[0];
	EXPECT_EQ("TA1", *rule1.getEU4Tag());
	EXPECT_EQ("GA1", *rule1.getV3Tag());
	const auto& rule12 = mapper.getMappingRules()[11];
	EXPECT_EQ("GA9", *rule12.getV3Tag());
	EXPECT_EQ("Name10", *rule12.getName());
}

TEST(Mappers_CountryMapperTests, TrivialMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { eu4 = TA1 vic3 = GA1 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("TA1");

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("GA1", v3Tag);
}

TEST(Mappers_CountryMapperTests, NameMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { eu4 = TA2 name = "Name2" vic3 = GA2 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("D99"); // this is invalid/unknown TAG.
	country->setLocalizationName("english", "Name2");

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("GA2", v3Tag);
}

TEST(Mappers_CountryMapperTests, SoloNameMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { name = "Name3" vic3 = GA3 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("D99"); // this is invalid/unknown TAG.
	country->setLocalizationName("english", "Name3");

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("GA3", v3Tag);
}

TEST(Mappers_CountryMapperTests, SoloFlagMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { flag = flag1 vic3 = GA4 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("D99"); // this is invalid/unknown TAG.
	country->addFlag("flag1");

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("GA4", v3Tag);
}

TEST(Mappers_CountryMapperTests, MultipleFlagMatchingMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { flag = flag1 flag = flag2 vic3 = GA4 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("D99");		// this is invalid/unknown TAG.
	country->addFlag("flag2"); // flag1 missing

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("GA4", v3Tag);
}

TEST(Mappers_CountryMapperTests, SoloReformMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { reform = reform1 vic3 = GA5 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("D99"); // this is invalid/unknown TAG.
	country->addReform("reform1");

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("GA5", v3Tag);
}

TEST(Mappers_CountryMapperTests, MultipleReformMatchingMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { reform = reform1 reform = reform2 vic3 = GA5 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("D99");			 // this is invalid/unknown TAG.
	country->addReform("reform2"); // reform1 missing

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("GA5", v3Tag);
}

TEST(Mappers_CountryMapperTests, MixAndMatchFlagsAndReformsMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { flag = flag1 reform = reform1 vic3 = GA6 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("D99");			 // this is invalid/unknown TAG.
	country->addFlag("flag1");		 // will trip on GA4 example if places after GA4 line. This is normal.
	country->addReform("reform1"); // will trip on GA5 example if placed after GA5 line. This is normal.

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("GA6", v3Tag);
}

TEST(Mappers_CountryMapperTests, NameFlagCodeRecordMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { name = "Name7" flag_code = fc7 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("D99"); // this is invalid/unknown TAG.
	country->setLocalizationName("english", "Name7");

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("X00", v3Tag);
	EXPECT_EQ("fc7", *mapper.getFlagCode("X00"));
}

TEST(Mappers_CountryMapperTests, SourceTagFlagCodeRecordMappingsWork)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { eu4 = TA8 flag_code = fc8 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("TA8");

	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("X00", v3Tag);
	EXPECT_EQ("fc8", *mapper.getFlagCode("X00"));
}

TEST(Mappers_CountryMapperTests, ZeroMatchesAssignsTag)
{
	mappers::CountryMapper mapper;

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("D99"); // this is invalid/unknown TAG.
	const auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("X00", v3Tag);
}

TEST(Mappers_CountryMapperTests, CountryWillGetSameTagIfReprocessed)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { eu4 = TA1 vic3 = GA1 }

	const auto country = std::make_shared<EU4::Country>();
	country->setTag("TA1");
	auto v3Tag = mapper.assignV3TagToEU4Country(country);

	EXPECT_EQ("GA1", v3Tag);

	v3Tag = mapper.assignV3TagToEU4Country(country);
	EXPECT_EQ("GA1", v3Tag);
}

TEST(Mappers_CountryMapperTests, TwoCountriesWontGetSameTag)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { eu4 = TA9 vic3 = GA9 }
	// link = { name = "Name10" vic3 = GA9 }

	const auto country1 = std::make_shared<EU4::Country>();
	country1->setTag("TA9");
	const auto country2 = std::make_shared<EU4::Country>();
	country2->setLocalizationName("english", "Name10");
	const auto v3Tag1 = mapper.assignV3TagToEU4Country(country1);
	const auto v3Tag2 = mapper.assignV3TagToEU4Country(country2); // assigned X00 since GA9 is taken.

	EXPECT_EQ("GA9", v3Tag1);
	EXPECT_EQ("X00", v3Tag2);
}

TEST(Mappers_CountryMapperTests, CountryWontGetReservedTag)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	// link = { eu4 = TA9 vic3 = GA9 }
	// link = { name = "Name10" vic3 = GA9 } <- should get X00 but won't.
	mapper.registerKnownVanillaV3Tag("X00"); // we do this when importing vanilla and modded V3 tags

	const auto country1 = std::make_shared<EU4::Country>();
	country1->setTag("TA9");
	const auto country2 = std::make_shared<EU4::Country>();
	country2->setLocalizationName("english", "Name10");
	const auto v3Tag1 = mapper.assignV3TagToEU4Country(country1);
	const auto v3Tag2 = mapper.assignV3TagToEU4Country(country2); // assigned X00 since GA9 is taken.

	EXPECT_EQ("GA9", v3Tag1);
	EXPECT_EQ("X01", v3Tag2); // gets the next free tag.
}

TEST(Mappers_CountryMapperTests, tagIsDynamicWorksAsAdvertised)
{
	const mappers::CountryMapper mapper;
	EXPECT_TRUE(mapper.tagIsDynamic("C01"));
	EXPECT_FALSE(mapper.tagIsDynamic("CC1"));
	EXPECT_FALSE(mapper.tagIsDynamic("CCC"));
	EXPECT_FALSE(mapper.tagIsDynamic("0C0"));
	EXPECT_FALSE(mapper.tagIsDynamic("0CC"));
	EXPECT_FALSE(mapper.tagIsDynamic("00C"));
}

TEST(Mappers_CountryMapperTests, tagIsNonCanonWorksAsAdvertised)
{
	const mappers::CountryMapper mapper;
	EXPECT_TRUE(mapper.tagIsNonCanon("C01"));
	EXPECT_TRUE(mapper.tagIsNonCanon("C0C"));
	EXPECT_FALSE(mapper.tagIsNonCanon("CC1"));
	EXPECT_FALSE(mapper.tagIsNonCanon("CCC"));
	EXPECT_FALSE(mapper.tagIsNonCanon("0C0"));
	EXPECT_FALSE(mapper.tagIsNonCanon("0CC"));
	EXPECT_FALSE(mapper.tagIsNonCanon("00C"));
}

TEST(Mappers_CountryMapperTests, newTagCanBeRequested)
{
	mappers::CountryMapper mapper;

	auto tag = mapper.requestNewV3Tag();
	EXPECT_EQ("X00", tag);
	tag = mapper.requestNewV3Tag();
	EXPECT_EQ("X01", tag);
	tag = mapper.requestNewV3Tag();
	EXPECT_EQ("X02", tag);
}

TEST(Mappers_CountryMapperTests, tagGeneratorWillSkipCollisions)
{
	const auto country1 = std::make_shared<EU4::Country>();
	country1->setTag("Y00"); // will map to X01

	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");
	auto tag = mapper.assignV3TagToEU4Country(country1);
	EXPECT_EQ("X01", tag);

	tag = mapper.requestNewV3Tag();
	EXPECT_EQ("X00", tag);
	tag = mapper.requestNewV3Tag();
	EXPECT_EQ("X02", tag);
}
