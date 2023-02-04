#include "CommonFunctions.h"
#include "LocalizationLoader/EU4LocalizationLoader.h"
#include "ReligionLoader/ReligionLoader.h"
#include "ReligionMapper/ReligionMapper.h"
#include "gmock/gmock-matchers.h"
#include "gtest/gtest.h"

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game/", {});
const auto eu4FS = commonItems::ModFilesystem("TestFiles/eu4installation/", {Mod("Some mod", "TestFiles/mod/themod/")});
} // namespace

TEST(Mappers_ReligionMapperTests, religionsCanBeRetrieved)
{
	mappers::ReligionMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_map.txt");

	EXPECT_EQ("catholic", *mapper.getV3Religion("catholic"));
	EXPECT_EQ("catholic", *mapper.getV3Religion("insular_celtic"));
	EXPECT_EQ("protestant", *mapper.getV3Religion("protestant"));
	EXPECT_EQ("protestant", *mapper.getV3Religion("reformed"));
	EXPECT_EQ("protestant", *mapper.getV3Religion("anglican"));
}

TEST(Mappers_ReligionMapperTests, MisMapReturnsNullopt)
{
	mappers::ReligionMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_map.txt");

	EXPECT_EQ(std::nullopt, mapper.getV3Religion("unmapped"));
}

TEST(Mappers_ReligionMapperTests, ReligionsMappingsCanBeExpanded)
{
	mappers::ReligionMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_map.txt");

	const std::map<std::string, EU4::Religion> additionalReligions = {{"additional1", EU4::Religion()}, {"additional2", EU4::Religion()}};

	mapper.expandReligionMappings(additionalReligions);

	EXPECT_EQ("additional1", mapper.getV3Religion("additional1"));
	EXPECT_EQ("additional2", mapper.getV3Religion("additional2"));
}

TEST(Mappers_ReligionMapperTests, ReligiousDefsCanBeGenerated)
{
	EU4::ReligionLoader theReligions;
	theReligions.loadReligions(eu4FS);

	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " religion_2: \"The Religion 2\"\n";
	input << " converted_dynamic_faith_107: \"The Religion 107\"\n";
	EU4::EU4LocalizationLoader locs;
	locs.loadLocalizations(input);

	mappers::ReligionMapper mapper;
	mapper.loadMappingRules("TestFiles/configurables/religion_map.txt");
	mapper.expandReligionMappings(theReligions.getAllReligions());
	mapper.generateReligionDefinitions(modFS, "TestFiles/configurables/religion_group_map.txt", theReligions.getAllReligions(), locs);

	// eu4 religion_2 is a default mapping into vic3's religion_2 which has preset defs:
	const auto& religion2 = mapper.getV3ReligionDefinitions().at("religion_2");
	EXPECT_EQ("religion_2", religion2.name);
	EXPECT_EQ("gfx/interface/icons/religion_icons/religion_2.dds", religion2.texture);
	EXPECT_THAT(religion2.traits, testing::UnorderedElementsAre("religiontrait_2"));
	EXPECT_EQ(commonItems::Color(std::array{4, 5, 6}), *religion2.color);
	EXPECT_TRUE(religion2.taboos.empty());
	EXPECT_TRUE(religion2.locBlock.empty()); // no locs since this is a preset def!

	// eu4 converted_dynamic_faith_107 is a dynamic mapping via groups (mod_group_2):
	// link = { vic3 = religiontrait_3 eu4 = mod_group_2 eu4 = mod_group_3 icon = religion_2 taboo = liquor taboo = wine }
	const auto& dyn7 = mapper.getV3ReligionDefinitions().at("converted_dynamic_faith_107");
	EXPECT_EQ("converted_dynamic_faith_107", dyn7.name);
	// link = { vic3 = religion_1 eu4 = shamanism }
	EXPECT_EQ("gfx/interface/icons/religion_icons/religion_1.dds", dyn7.texture); // from shamanism trappings.
	EXPECT_THAT(dyn7.traits, testing::UnorderedElementsAre("religiontrait_3"));	// from group
	EXPECT_EQ(commonItems::Color(std::array{135, 55, 140}), *dyn7.color);			// from eu4 defs
	EXPECT_THAT(dyn7.taboos, testing::UnorderedElementsAre("liquor", "wine"));		// from group
	EXPECT_EQ("The Religion 107", dyn7.locBlock.at("english"));
}
