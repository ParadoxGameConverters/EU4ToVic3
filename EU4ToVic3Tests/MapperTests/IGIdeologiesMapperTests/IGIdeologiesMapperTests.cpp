#include "ClayManager/ClayManager.h"
#include "CountryManager/EU4Country.h"
#include "CultureMapper/CultureMapper.h"
#include "IGIdeologiesMapper/IGIdeologiesMapper.h"
#include "PoliticalManager/Country/Country.h"
#include "ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_IGIdeologiesMapperTests, NoLinksProvidesNothing)
{
	const mappers::IGIdeologiesMapper mapper;

	const auto country = std::make_shared<V3::Country>();
	const auto eu4Country = std::make_shared<EU4::Country>();
	country->setSourceCountry(eu4Country);

	const auto mods = mapper.getIGIdeologyMods(*country, {}, {}, {});

	EXPECT_TRUE(mods.empty());
}

TEST(Mappers_IGIdeologiesMapperTests, modsCanBeGenerated)
{
	mappers::IGIdeologiesMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/ig_ideologies.txt");

	// prep srccountry
	std::stringstream eu4input;
	eu4input << "government = { government = monarchy } active_idea_groups = { aristocracy_ideas = 7 } religion = orthodox";
	const auto eu4Country = std::make_shared<EU4::Country>("TAG", eu4input);

	// prep culture
	auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);
	V3::ProcessedData data;
	data.cultures = {"japanese", "russian"};
	country->setProcessedData(data);

	// prep cultural trait
	mappers::CultureMapper culMapper;
	mappers::CultureDef def;
	def.name = "japanese";
	def.traits.emplace("yamato");
	culMapper.setV3CultureDefinitions({std::pair("japanese", def)});

	// match
	auto mods = mapper.getIGIdeologyMods(*country, culMapper, {}, {});

	EXPECT_THAT(mods.at("ig_armed_forces").addedIdeologies, testing::UnorderedElementsAre("ideology_bakufu"));
	EXPECT_THAT(mods.at("ig_armed_forces").removedIdeologies, testing::UnorderedElementsAre());
	EXPECT_THAT(mods.at("ig_devout").addedIdeologies, testing::UnorderedElementsAre("ideology_russian_patriarch"));
	EXPECT_THAT(mods.at("ig_devout").removedIdeologies, testing::UnorderedElementsAre("ideology_patriarchal"));
}
