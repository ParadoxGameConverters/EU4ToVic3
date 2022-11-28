#include "ClayManager/Chunk.h"
#include "ProvinceManager/ProvinceManager.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

TEST(V3World_ChunkTests, DemographicsCanBeImported)
{
	// make a few provinces
	std::stringstream provinceStream;
	provinceStream << "-2={ owner = TA2 base_tax=10 base_production=10 base_manpower=10 culture = culture religion = religion}\n"; // substate TA-2&3
	provinceStream << "-3={ owner = TA3 base_tax=1 base_production=1 base_manpower=1 culture = culture religion = religion }\n";	 // substate TA-2&3
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.buildProvinceWeights();
	provinceManager.buildPopRatios({}, false);

	// shove provinces into a chunk.
	V3::Chunk chunk;
	chunk.addSourceProvince(std::pair(2, provinceManager.getProvince(2)));
	chunk.addSourceProvince(std::pair(3, provinceManager.getProvince(3)));

	// and import demos.

	chunk.importDemographics();

	ASSERT_EQ(2, chunk.getDemographics().size());
	const auto& demo1 = chunk.getDemographics()[0];
	const auto& demo2 = chunk.getDemographics()[1];
	EXPECT_EQ("culture", demo1.culture);
	EXPECT_EQ("culture", demo2.culture);
	EXPECT_EQ("religion", demo1.religion);
	EXPECT_EQ("religion", demo2.religion);
	EXPECT_NEAR(0.909, demo1.upperRatio, 0.001);	 // 10-1 dev ratio
	EXPECT_NEAR(0.091, demo2.upperRatio, 0.001);	 // 1-10 dev ratio
	EXPECT_NEAR(0.909, demo1.middleRatio, 0.001); // 10-1 dev ratio
	EXPECT_NEAR(0.091, demo2.middleRatio, 0.001); // 1-10 dev ratio
	EXPECT_NEAR(0.909, demo1.lowerRatio, 0.001);	 // 10-1 dev ratio
	EXPECT_NEAR(0.091, demo2.lowerRatio, 0.001);	 // 1-10 dev ratio
}
