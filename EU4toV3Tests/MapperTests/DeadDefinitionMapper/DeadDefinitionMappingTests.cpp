#include "DeadDefinitionMapper/DeadDefinitionMapping.h"
#include "gtest/gtest.h"

TEST(Mappers_DeadDefinitionMappingTests, primitivesDefaultToDefaults)
{
	std::stringstream input;
	const mappers::DeadDefinitionMapping mapping(input);

	ASSERT_TRUE(mapping.getDefinition().tag.empty());
	ASSERT_FALSE(mapping.getDefinition().culture);
	ASSERT_FALSE(mapping.getDefinition().religion);
	ASSERT_FALSE(mapping.getDefinition().government);
	ASSERT_FALSE(mapping.getDefinition().civilized);
	ASSERT_FALSE(mapping.getDefinition().capital);
	ASSERT_TRUE(mapping.getDefinition().acceptedCultures.empty());
}

TEST(Mappers_DeadDefinitionMappingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "tag = AAA\n";
	input << "primary_culture = dummy\n";
	input << "religion = pre_schism\n";
	input << "government = theocracy\n";
	input << "civilized = yes\n";
	input << "capital = 322\n";
	input << "add_accepted_culture = culture1\n";
	input << "add_accepted_culture = culture2\n";
	input << "add_accepted_culture = culture3\n";
	const mappers::DeadDefinitionMapping mapping(input);

	ASSERT_EQ("AAA", mapping.getDefinition().tag);
	ASSERT_EQ("dummy", *mapping.getDefinition().culture);
	ASSERT_EQ("pre_schism", *mapping.getDefinition().religion);
	ASSERT_EQ("theocracy", *mapping.getDefinition().government);
	ASSERT_TRUE(*mapping.getDefinition().civilized);
	ASSERT_EQ(322, *mapping.getDefinition().capital);
	ASSERT_EQ(3, mapping.getDefinition().acceptedCultures.size());
	ASSERT_TRUE(mapping.getDefinition().acceptedCultures.contains("culture1"));
	ASSERT_TRUE(mapping.getDefinition().acceptedCultures.contains("culture2"));
	ASSERT_TRUE(mapping.getDefinition().acceptedCultures.contains("culture3"));
}
