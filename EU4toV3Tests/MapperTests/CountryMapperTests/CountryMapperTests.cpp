#include "CountryManager/EU4Country.h"
#include "CountryMapper/CountryMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_CountryMapperTests, rulesCanBeLoadedInOrder)
{
	mappers::CountryMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/country_mappings.txt");

	EXPECT_EQ(10, mapper.getMappingRules().size());
	const auto& rule1 = mapper.getMappingRules()[0];
	EXPECT_EQ("TA1", *rule1.getEU4Tag());
	EXPECT_EQ("GA1", *rule1.getV3Tag());
	const auto& rule10 = mapper.getMappingRules()[9];
	EXPECT_EQ("TA8", *rule10.getEU4Tag());
	EXPECT_EQ("fc8", *rule10.getFlagCode());
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

TEST(Mappers_CountryMapperTests, tagIsAlphaDigitDigitWorksAsAdvertised)
{
	EXPECT_TRUE(mappers::CountryMapper::tagIsAlphaDigitDigit("C01"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("CC1"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("CCC"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("0C0"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("0CC"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitDigit("00C"));
}

TEST(Mappers_CountryMapperTests, tagIsAlphaDigitAlphaNumWorksAsAdvertised)
{
	EXPECT_TRUE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("C01"));
	EXPECT_TRUE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("C0C"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("CC1"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("CCC"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("0C0"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("0CC"));
	EXPECT_FALSE(mappers::CountryMapper::tagIsAlphaDigitAlphaNum("00C"));
}
