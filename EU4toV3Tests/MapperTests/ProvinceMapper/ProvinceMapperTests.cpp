#include "Configuration.h"
#include "ProvinceMappings/ProvinceMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_ProvinceMapperTests, correctMappingVersionIsUsed)
{
	std::stringstream colonialInput;
	std::stringstream input;
	input << "1.2.3 = {\n";
	input << "\tlink = { eu4 = 1 v2 = 1 }\n";
	input << "}\n";
	input << "1.0.1.4 = {\n";
	input << "\tlink = { eu4 = 1 v2 = 2 }\n";
	input << "}\n";
	input << "1.1.1 = {\n";
	input << "\tlink = { eu4 = 1 v2 = 3 }\n"; // <- We'll be using this one since it's best match for version 1.1.3 defined below.
	input << "}\n";
	input << "1.0.0.0 = {\n";
	input << "\tlink = { eu4 = 1 v2 = 4 }\n";
	input << "}\n";

	Configuration testConfiguration;
	testConfiguration.setEU4Version(GameVersion("1.1.3"));
	const mappers::ProvinceMapper mapper(input, colonialInput, testConfiguration);

	std::set<int> expected = {3};
	ASSERT_EQ(expected, mapper.getVic2ProvinceNumbers(1));
}

TEST(Mappers_ProvinceMapperTests, canLookupVic2Provinces)
{
	std::stringstream colonialInput;
	std::stringstream input;
	input << "0.0.0.0 = {\n";
	input << "\tlink = { eu4 = 2 eu4 = 1 v2 = 2 v2 = 1 }\n";
	input << "}";

	const Configuration testConfiguration;
	const mappers::ProvinceMapper mapper(input, colonialInput, testConfiguration);

	ASSERT_EQ(2, mapper.getVic2ProvinceNumbers(1).size());
	ASSERT_TRUE(mapper.getVic2ProvinceNumbers(1).contains(1));
	ASSERT_TRUE(mapper.getVic2ProvinceNumbers(1).contains(2));

	ASSERT_EQ(2, mapper.getVic2ProvinceNumbers(2).size());
	ASSERT_TRUE(mapper.getVic2ProvinceNumbers(2).contains(1));
	ASSERT_TRUE(mapper.getVic2ProvinceNumbers(2).contains(2));
}

TEST(Mappers_ProvinceMapperTests, canLookupEU4Provinces)
{
	std::stringstream colonialInput;
	std::stringstream input;
	input << "0.0.0.0 = {\n";
	input << "\tlink = { eu4 = 2 eu4 = 1 v2 = 2 v2 = 1 }\n";
	input << "}";

	const Configuration testConfiguration;
	const mappers::ProvinceMapper mapper(input, colonialInput, testConfiguration);

	ASSERT_EQ(2, mapper.getEU4ProvinceNumbers(1).size());
	ASSERT_TRUE(mapper.getEU4ProvinceNumbers(1).contains(1));
	ASSERT_TRUE(mapper.getEU4ProvinceNumbers(1).contains(2));

	ASSERT_EQ(2, mapper.getEU4ProvinceNumbers(2).size());
	ASSERT_TRUE(mapper.getEU4ProvinceNumbers(2).contains(1));
	ASSERT_TRUE(mapper.getEU4ProvinceNumbers(2).contains(2));
}

TEST(Mappers_ProvinceMapperTests, colonialRegionIsForwardedFromColonialRegionsMapper)
{
	std::stringstream colonialInput;
	colonialInput << "colonial_alaska = {\n";
	colonialInput << "\tprovinces = {\n";
	colonialInput << "\t\t1 2 3\n";
	colonialInput << "\t}\n";
	colonialInput << "}\n";

	std::stringstream input;
	input << "0.0.0.0 = {\n";
	input << "\tlink = { eu4 = 2 eu4 = 1 v2 = 2 v2 = 1 }\n";
	input << "}";

	const Configuration testConfiguration;
	const mappers::ProvinceMapper mapper(input, colonialInput, testConfiguration);

	ASSERT_EQ("colonial_alaska", mapper.getColonialRegionForProvince(1));
	ASSERT_EQ(std::nullopt, mapper.getColonialRegionForProvince(4));
}
