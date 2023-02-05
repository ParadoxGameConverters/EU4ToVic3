#include "CultureMapper/NameListMapper/NameListMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_NameListMappingTests, primitivesDefaultsToBlank)
{
	std::stringstream input;
	const mappers::NameListMapping mapping(input);

	EXPECT_TRUE(mapping.getCulture().empty());
	EXPECT_TRUE(mapping.getCultureGroup().empty());
	EXPECT_TRUE(mapping.getNamePool().empty());
}

TEST(Mappers_NameListMappingTests, primitivesCanBeLoaded)
{
	std::stringstream input;
	input << "eu4 = culture1 eu4group = group1 name_pool = name_list_test1";
	const mappers::NameListMapping mapping(input);

	EXPECT_EQ("culture1", mapping.getCulture());
	EXPECT_EQ("group1", mapping.getCultureGroup());
	EXPECT_EQ("name_list_test1", mapping.getNamePool());
}
