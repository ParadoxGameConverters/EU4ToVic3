#include "CultureGroups/Culture.h"
#include "CultureGroups/CultureGroup.h"
#include "CultureGroups/CultureGroups.h"
#include "CultureMapper/CultureMapper.h"
#include "Regions/Areas.h"
#include "Regions/Regions.h"
#include "Regions/SuperRegions.h"
#include "gtest/gtest.h"
TEST(Mappers_CultureGroupsTests, cultureGroupsCanBeLoaded)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	const mappers::CultureGroups groups(input);

	ASSERT_EQ(2, groups.getCultureGroupsMap().size());
	ASSERT_TRUE(groups.getCultureGroupsMap().contains("groupA"));
	ASSERT_TRUE(groups.getCultureGroupsMap().contains("groupB"));
}

TEST(Mappers_CultureGroupsTests, groupCanBeRetrieved)
{
	std::stringstream input;
	input << "groupA = { cultureA = {} }\n";
	input << "groupB = { cultureB = {} }\n";
	const mappers::CultureGroups groups(input);

	const auto& groupA = groups.getGroupForCulture("cultureA");

	ASSERT_EQ("groupA", groupA->getName());
	ASSERT_TRUE(groupA->containsCulture("cultureA"));
}

TEST(Mappers_CultureGroupsTests, groupMismatchReturnsNullptr)
{
	std::stringstream input;
	const mappers::CultureGroups groups(input);

	const auto& groupC = groups.getGroupForCulture("cultureC");

	ASSERT_EQ(nullptr, groupC);
}

TEST(Mappers_CultureGroupsTests, neoculturesCanBeImportedFromEU4IntoV2)
{
	// To comprehend this test, remember that "euculture" is eu4 english, "vculture" is v2 english,
	// and "neoculture" is eu4/v2 dynamically-constructed-yankee which lacks any mapping to any v2 culture.

	// these are generic regions needed for cultureMapper.
	std::stringstream superRegionsInput;
	superRegionsInput << "test_superregion = {";
	superRegionsInput << "\ttest_region";
	superRegionsInput << "}";
	EU4::SuperRegions superRegions(superRegionsInput);

	std::stringstream regionsInput;
	regionsInput << "test_region = {\n";
	regionsInput << "\tareas = {\n";
	regionsInput << "\t\ttest_area\n";
	regionsInput << "\t}\n";
	regionsInput << "}";

	std::stringstream areasInput;
	areasInput << "test_area = {\n";
	areasInput << "\t1 2 3\n";
	areasInput << "}";
	EU4::Areas theAreas(areasInput);
	EU4::Regions theRegions(superRegions, theAreas, regionsInput);

	// This is our culturemapper. Note there is nothing dynamic in it.
	std::stringstream cultureInput;
	cultureInput << "link = { eu4 = euculture vic2 = vculture }";
	mappers::CultureMapper cultureMapper(cultureInput);

	// And our eu4 culture groups mapper. No funny stuff.
	std::stringstream eu4input;
	eu4input << "eugroup = { euculture = {} }";
	auto eu4groups = std::make_shared<mappers::CultureGroups>(eu4input);

	// We need to create our eu4 neoculture first.
	const auto& euGroup = eu4groups->getGroupForCulture("euculture");
	const auto& euCulture = euGroup->getCultures().find("euculture")->second;
	euGroup->addNeoCulture("neoculture", euCulture, "euculture"); // we created a neoculture from our euculture.

	// This is our V2 culture group mapper. Has nothing to do with the former, yet.
	std::stringstream v2input;
	v2input << "vgroup = { vculture = {} }";
	mappers::CultureGroups v2groups(v2input);

	// We're now importing an eu4 culture "neoculture" into our mapper, without it having a dedicated link in the cultureMapper.
	// This is the whole point of the exercise.
	v2groups.importNeoCultures(theRegions, eu4groups, cultureMapper);

	// Now let's grab the target group
	const auto& vgroup = v2groups.getGroupForCulture("neoculture");
	// vgroup now has 2 cultures - the vculture which euculture maps to, along with neoculture which tags along.

	ASSERT_EQ(2, vgroup->getCultures().size());
	ASSERT_TRUE(vgroup->containsCulture("vculture"));
	ASSERT_TRUE(vgroup->containsCulture("neoculture"));

	// And, of course:
	const auto& v2NeoCulture = vgroup->getCultures().find("neoculture")->second;
	ASSERT_TRUE(v2NeoCulture->isNeoCulture());
}


TEST(Mappers_CultureGroupsTests, v2CultureGroupsCanBeOutput)
{
	std::stringstream input;
	input << "someculturegroup = {\n";
	input << "\tleader = european\n";
	input << "\tis_overseas = yes\n";
	input << "\tunit = MiddleEasternGC\n";
	input << "\tunion = TAG\n";
	input << "\tsomeculture = {\n";
	input << "\t\tprimary = TAG\n";
	input << "\t\tfirst_names = { Bob Jon }\n";
	input << "\t\tlast_names = { Bobby Johnny }\n";
	input << "\t\tradicalism = 4\n";
	input << "\t\tcolor = { 1 2 3 }\n";
	input << "\t}\n";
	input << "}\n";
	mappers::CultureGroups groups(input);

	std::stringstream output;
	output << "someculturegroup = {\n";
	output << "\tleader = european\n";
	output << "\tis_overseas = yes\n";
	output << "\tunit = MiddleEasternGC\n";
	output << "\t\n";
	output << "\tsomeculture = {\n";
	output << "\t\tcolor = { 1 2 3 }\n";
	output << "\t\tradicalism = 4\n";
	output << "\t\tprimary = TAG\n";
	output << "\t\t\n";
	output << "\t\tfirst_names = { Bob Jon }\n";
	output << "\t\t\n";
	output << "\t\tlast_names = { Bobby Johnny }\n";
	output << "\t\t\n";
	output << "\t}\n";
	output << "\t\n";
	output << "\tunion = TAG\n";
	output << "\t\n";
	output << "}\n\n";

	std::stringstream actual;
	actual << groups;

	ASSERT_EQ(output.str(), actual.str());
}
