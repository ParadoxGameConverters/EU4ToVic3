#include "ClayManager/ClayManager.h"
#include "CountryManager/EU4Country.h"
#include "CultureMapper/CultureMapper.h"
#include "IGIdeologiesMapper/IGIdeologiesMapping.h"
#include "PoliticalManager/Country/Country.h"
#include "ReligionMapper/ReligionMapper.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_IGIdeologiesMappingTests, EmptyLinkAltersNothing)
{
	std::map<std::string, mappers::IGIdeologyMod> mods;

	std::stringstream input;
	const mappers::IGIdeologiesMapping mapping(input);

	const auto country = std::make_shared<V3::Country>();
	const auto eu4Country = std::make_shared<EU4::Country>();
	country->setSourceCountry(eu4Country);

	mods = mapping.alterIdeologyMods(mods, *country, {}, {}, {});

	EXPECT_TRUE(mods.empty());
}

TEST(Mappers_IGIdeologiesMappingTests, LinkCanAlterMods)
{
	std::map<std::string, mappers::IGIdeologyMod> mods;

	std::stringstream input;
	input << "culturetrait = yamato eu4gov = monarchy eu4gov = theocracy ig_armed_forces = add:ideology_bakufu ig_armed_forces = add:ideology_paternalistic";
	const mappers::IGIdeologiesMapping mapping(input);

	// prep gov
	std::stringstream eu4input;
	eu4input << "government = { government = monarchy }";
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
	mods = mapping.alterIdeologyMods(mods, *country, culMapper, {}, {});

	ASSERT_EQ(1, mods.size());
	EXPECT_THAT(mods.at("ig_armed_forces").addedIdeologies, testing::UnorderedElementsAre("ideology_bakufu", "ideology_paternalistic"));
}

TEST(Mappers_IGIdeologiesMappingTests, LinkCanNullifyExistingMods)
{
	std::map<std::string, mappers::IGIdeologyMod> mods;
	mods.emplace("ig_armed_forces", mappers::IGIdeologyMod());
	mods.at("ig_armed_forces").removedIdeologies.emplace("ideology_bakufu");

	std::stringstream input;
	input << "culturetrait = yamato eu4gov = monarchy eu4gov = theocracy ig_armed_forces = add:ideology_bakufu ig_armed_forces = add:ideology_paternalistic";
	const mappers::IGIdeologiesMapping mapping(input);

	// prep gov
	std::stringstream eu4input;
	eu4input << "government = { government = monarchy }";
	const auto eu4Country = std::make_shared<EU4::Country>("TAG", eu4input);

	// prep culture
	auto country = std::make_shared<V3::Country>();
	country->setSourceCountry(eu4Country);
	V3::ProcessedData data;
	data.cultures = {"japanese"};
	country->setProcessedData(data);

	// prep cultural trait
	mappers::CultureMapper culMapper;
	mappers::CultureDef def;
	def.name = "japanese";
	def.traits.emplace("yamato");
	culMapper.setV3CultureDefinitions({std::pair("japanese", def)});

	// match
	mods = mapping.alterIdeologyMods(mods, *country, culMapper, {}, {});

	ASSERT_EQ(1, mods.size());
	EXPECT_THAT(mods.at("ig_armed_forces").addedIdeologies, testing::UnorderedElementsAre("ideology_paternalistic"));
}
