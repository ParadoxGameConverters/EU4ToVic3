#include "ProvinceMappings/ProvinceMapping.h"
#include "ProvinceMappings/ProvinceMappingsVersion.h"
#include "gtest/gtest.h"

TEST(Mappers_ProvinceMappingsVersionTests, VersionCanBeSet)
{
	const std::string versionString = "12.11.10";

	std::stringstream input;
	const mappers::ProvinceMappingsVersion mapper(versionString, input);

	GameVersion version(versionString);
	ASSERT_EQ(version, mapper.getVersion());
}

TEST(Mappers_ProvinceMappingsVersionTests, MappingsDefaultToEmpty)
{
	const std::string versionString = "12.11.10";

	std::stringstream input;
	const mappers::ProvinceMappingsVersion mapper(versionString, input);

	ASSERT_TRUE(mapper.getMappings().empty());
}

TEST(Mappers_ProvinceMappingsVersionTests, MappingsCanBeLoaded)
{
	const std::string versionString = "12.11.10";

	std::stringstream input;
	input << "link = { eu4 = 90 eu4 = 1 v2 = 1 }\n";
	input << "link = { eu4 = 2 v2 = 2 v2 = 3}\n";
	const mappers::ProvinceMappingsVersion mapper(versionString, input);

	ASSERT_EQ(2, mapper.getMappings().size());
	ASSERT_TRUE(mapper.getMappings()[0].getEU4Provinces().contains(90));
	ASSERT_TRUE(mapper.getMappings()[0].getEU4Provinces().contains(1));
	ASSERT_TRUE(mapper.getMappings()[0].getVic2Provinces().contains(1));

	ASSERT_TRUE(mapper.getMappings()[1].getEU4Provinces().contains(2));
	ASSERT_TRUE(mapper.getMappings()[1].getVic2Provinces().contains(2));
	ASSERT_TRUE(mapper.getMappings()[1].getVic2Provinces().contains(3));
}
