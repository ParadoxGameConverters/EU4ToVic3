#include "CharacterTraitMapper/AssignmentMapping.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(Mappers_AssignmentMappingTests, DefaultsDefaultToDefaults)
{

	std::stringstream input;
	const mappers::AssignmentMapping mapping(input);

	EXPECT_TRUE(mapping.getAssignments().empty());
}

TEST(Mappers_AssignmentMappingTests, AssignmentsCanBeLoaded)
{
	std::stringstream input;
	input << "a = a1\n";
	input << "b = b2\n";
	const mappers::AssignmentMapping mapping(input);

	EXPECT_THAT(mapping.getAssignments(), testing::UnorderedElementsAre(std::pair("a", "a1"), std::pair("b", "b2")));
}
