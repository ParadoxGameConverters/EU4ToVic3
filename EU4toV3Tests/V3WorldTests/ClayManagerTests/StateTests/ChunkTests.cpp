#include "ClayManager/State/Chunk.h"
#include "ProvinceManager/ProvinceManager.h"
#include "gtest/gtest.h"

TEST(V3World_ChunkTests, SourceProvinceDataCanBeAdded)
{
	// make a few provinces
	std::stringstream provinceStream;
	provinceStream << "-2={ owner = TA2 base_tax=10 base_production=10 base_manpower=10 culture = culture religion = religion }\n"; //weight 30
	provinceStream << "-3={ owner = TA3 base_tax=1 base_production=1 base_manpower=1 culture = culture2 religion = religion2 }\n"; // weight 3
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.buildProvinceWeights();
	provinceManager.buildPopRatios({}, false);

	// shove provinces into a chunk.
	V3::Chunk chunk;
	chunk.addSourceProvinceData(provinceManager.getProvince(2), 30);
	chunk.addSourceProvinceData(provinceManager.getProvince(3), 3);

	const auto& spdata = chunk.getSourceProvinceData();
	ASSERT_EQ(2, spdata.size());
	const auto& [sp1, sp1weight] = spdata[0];
	const auto& [sp2, sp2weight] = spdata[1];

	EXPECT_EQ("TA2", sp1.owner);
	EXPECT_DOUBLE_EQ(30, sp1.weight);
	ASSERT_EQ(1, sp1.popRatios.size());
	EXPECT_EQ("culture", sp1.popRatios[0].getCulture());
	EXPECT_EQ("religion", sp1.popRatios[0].getReligion());
	EXPECT_DOUBLE_EQ(30, sp1weight);

	EXPECT_EQ("TA3", sp2.owner);
	EXPECT_DOUBLE_EQ(3, sp2.weight);
	ASSERT_EQ(1, sp2.popRatios.size());
	EXPECT_EQ("culture2", sp2.popRatios[0].getCulture());
	EXPECT_EQ("religion2", sp2.popRatios[0].getReligion());
	EXPECT_DOUBLE_EQ(3, sp2weight);
}
