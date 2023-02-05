#include "CultureMapper/NameListLoader/NameListLoader.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_NameListLoaderTests, religionsCanBeRetrieved)
{
	mappers::NameListLoader loader;
	loader.loadNameLists("TestFiles/configurables/name_lists.txt");

	const auto& nameList = loader.getNameList("name_list_test2");
	ASSERT_TRUE(nameList);

	EXPECT_THAT(nameList->getDynastyNames(), testing::UnorderedElementsAre("dyn3", "dyn4"));
	EXPECT_THAT(nameList->getMaleNames(), testing::UnorderedElementsAre("male3", "male4"));
	EXPECT_THAT(nameList->getFemaleNames(), testing::UnorderedElementsAre("female3", "female4"));
}

TEST(Mappers_NameListLoaderTests, MismatchReturnsNullopt)
{
	mappers::NameListLoader loader;
	loader.loadNameLists("TestFiles/configurables/name_lists.txt");

	const auto& nameList = loader.getNameList("nonsense");
	ASSERT_FALSE(nameList);
}
