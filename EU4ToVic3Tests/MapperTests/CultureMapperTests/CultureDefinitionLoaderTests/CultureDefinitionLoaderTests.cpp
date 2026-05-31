#include "CultureMapper/CultureDefinitionLoader/CultureDefinitionLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
#include <sstream>

namespace
{
const auto modFS = commonItems::ModFilesystem("TestFiles/vic3installation/game", {});

void loadDefinition(mappers::CultureDefinitionLoader& loader, const std::string& definition)
{
	std::stringstream input(definition);
	loader.loadDefinitions(input);
}
}

TEST(Mappers_CultureDefinitionLoaderTests, LoaderDefaultsToDry)
{
	const mappers::CultureDefinitionLoader loader;
	ASSERT_TRUE(loader.getDefinitions().empty());
}

TEST(Mappers_CultureDefinitionLoaderTests, CulturesCanBeRetrieved)
{
	mappers::CultureDefinitionLoader loader;
	loader.loadDefinitions(modFS);

	ASSERT_TRUE(loader.getDefinitions().contains("vculture2"));
	const auto& vculture2 = loader.getDefinitions().at("vculture2");

	EXPECT_EQ("vculture2", vculture2.name);
	EXPECT_THAT(vculture2.ethnicities, testing::UnorderedElementsAre("ethnotest2"));
}

TEST(Mappers_CultureDefinitionLoaderTests, InjectMergesIntoExistingCulture)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(gondi = {
			religion = religion_base
			heritage = heritage_base
			language = language_base
			traditions = { tradition_base }
			male_common_first_names = { base_name }
			ethnicities = { 1 = ethnicity_base }
			graphics = graphics_base
		})");
	loadDefinition(loader,
		 R"(INJECT:gondi = {
			religion = religion_injected
			traditions = { tradition_injected }
			male_common_first_names = { injected_name }
			ethnicities = { 1 = ethnicity_injected }
		})");

	const auto& gondi = loader.getDefinitions().at("gondi");
	EXPECT_EQ("religion_injected", gondi.religion);
	EXPECT_EQ("heritage_base", gondi.heritage);
	EXPECT_EQ("language_base", gondi.language);
	EXPECT_EQ("graphics_base", gondi.graphics);
	EXPECT_THAT(gondi.traditions, testing::UnorderedElementsAre("tradition_base", "tradition_injected"));
	EXPECT_THAT(gondi.maleCommonFirstNames, testing::UnorderedElementsAre("base_name", "injected_name"));
	EXPECT_THAT(gondi.ethnicities, testing::UnorderedElementsAre("ethnicity_base", "ethnicity_injected"));
}

TEST(Mappers_CultureDefinitionLoaderTests, ReplaceOverwritesExistingCulture)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(gondi = {
			religion = religion_base
			heritage = heritage_base
			traditions = { tradition_base }
			male_common_first_names = { base_name }
			graphics = graphics_base
		})");
	loadDefinition(loader,
		 R"(REPLACE:gondi = {
			religion = religion_replaced
			graphics = graphics_replaced
		})");

	const auto& gondi = loader.getDefinitions().at("gondi");
	EXPECT_EQ("religion_replaced", gondi.religion);
	EXPECT_TRUE(gondi.heritage.empty());
	EXPECT_TRUE(gondi.traditions.empty());
	EXPECT_TRUE(gondi.maleCommonFirstNames.empty());
	EXPECT_EQ("graphics_replaced", gondi.graphics);
}

TEST(Mappers_CultureDefinitionLoaderTests, TryInjectMergesIntoExistingCulture)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(gondi = {
			language = language_base
			common_last_names = { base_last_name }
		})");
	loadDefinition(loader,
		 R"(TRY_INJECT:gondi = {
			language = language_injected
			common_last_names = { injected_last_name }
		})");

	const auto& gondi = loader.getDefinitions().at("gondi");
	EXPECT_EQ("language_injected", gondi.language);
	EXPECT_THAT(gondi.commonLastNames, testing::UnorderedElementsAre("base_last_name", "injected_last_name"));
}

TEST(Mappers_CultureDefinitionLoaderTests, TryReplaceOverwritesExistingCulture)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(gondi = {
			heritage = heritage_base
			female_common_first_names = { base_name }
		})");
	loadDefinition(loader,
		 R"(TRY_REPLACE:gondi = {
			heritage = heritage_replaced
		})");

	const auto& gondi = loader.getDefinitions().at("gondi");
	EXPECT_EQ("heritage_replaced", gondi.heritage);
	EXPECT_TRUE(gondi.femaleCommonFirstNames.empty());
}

TEST(Mappers_CultureDefinitionLoaderTests, InjectOnMissingCultureDoesNotCreateEntry)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(INJECT:missing_culture = {
			religion = religion_injected
		})");

	EXPECT_FALSE(loader.getDefinitions().contains("missing_culture"));
}

TEST(Mappers_CultureDefinitionLoaderTests, ReplaceOnMissingCultureDoesNotCreateEntry)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(REPLACE:missing_culture = {
			religion = religion_replaced
		})");

	EXPECT_FALSE(loader.getDefinitions().contains("missing_culture"));
}

TEST(Mappers_CultureDefinitionLoaderTests, TryInjectOnMissingCultureDoesNotCreateEntry)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(TRY_INJECT:missing_culture = {
			religion = religion_injected
		})");

	EXPECT_FALSE(loader.getDefinitions().contains("missing_culture"));
}

TEST(Mappers_CultureDefinitionLoaderTests, TryReplaceOnMissingCultureDoesNotCreateEntry)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(TRY_REPLACE:missing_culture = {
			religion = religion_replaced
		})");

	EXPECT_FALSE(loader.getDefinitions().contains("missing_culture"));
}

TEST(Mappers_CultureDefinitionLoaderTests, InjectOrCreateInjectsIntoExistingCulture)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(gondi = {
			religion = religion_base
			noble_last_names = { base_name }
		})");
	loadDefinition(loader,
		 R"(INJECT_OR_CREATE:gondi = {
			religion = religion_injected
			noble_last_names = { injected_name }
		})");

	const auto& gondi = loader.getDefinitions().at("gondi");
	EXPECT_EQ("religion_injected", gondi.religion);
	EXPECT_THAT(gondi.nobleLastNames, testing::UnorderedElementsAre("base_name", "injected_name"));
}

TEST(Mappers_CultureDefinitionLoaderTests, InjectOrCreateCreatesMissingCulture)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(INJECT_OR_CREATE:new_culture = {
			religion = religion_created
			traditions = { created_tradition }
		})");

	ASSERT_TRUE(loader.getDefinitions().contains("new_culture"));
	const auto& newCulture = loader.getDefinitions().at("new_culture");
	EXPECT_EQ("religion_created", newCulture.religion);
	EXPECT_THAT(newCulture.traditions, testing::UnorderedElementsAre("created_tradition"));
}

TEST(Mappers_CultureDefinitionLoaderTests, ReplaceOrCreateReplacesExistingCulture)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(gondi = {
			religion = religion_base
			regal_last_names = { base_name }
		})");
	loadDefinition(loader,
		 R"(REPLACE_OR_CREATE:gondi = {
			religion = religion_replaced
		})");

	const auto& gondi = loader.getDefinitions().at("gondi");
	EXPECT_EQ("religion_replaced", gondi.religion);
	EXPECT_TRUE(gondi.regalLastNames.empty());
}

TEST(Mappers_CultureDefinitionLoaderTests, ReplaceOrCreateCreatesMissingCulture)
{
	mappers::CultureDefinitionLoader loader;
	loadDefinition(loader,
		 R"(REPLACE_OR_CREATE:new_culture = {
			heritage = heritage_created
			female_regal_first_names = { created_name }
		})");

	ASSERT_TRUE(loader.getDefinitions().contains("new_culture"));
	const auto& newCulture = loader.getDefinitions().at("new_culture");
	EXPECT_EQ("heritage_created", newCulture.heritage);
	EXPECT_THAT(newCulture.femaleRegalFirstNames, testing::UnorderedElementsAre("created_name"));
}
