#include "ClayManager/State/Chunk.h"
#include "ProvinceManager/ProvinceManager.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>

namespace
{
const auto eu4FS = commonItems::ModFilesystem("TestFiles/eu4installation/", {});

V3::Chunk prepChunk()
{
	// make a few provinces
	std::stringstream provinceStream;
	provinceStream << "-2={ owner = TA2 base_tax=9 base_production=9 base_manpower=9 culture = culture religion = religion }\n";	 // weight 27
	provinceStream << "-3={ owner = TA3 base_tax=1 base_production=1 base_manpower=1 culture = culture2 religion = religion2 }\n"; // weight 3
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provinceStream);
	provinceManager.getProvince(2)->addCapital("TA2"); // we're transfering this as well.
	provinceManager.getProvince(3)->addCapital("TA3"); // we're making both capitals to make things dramatic.
	EU4::RegionManager regionManager;
	regionManager.loadRegions(eu4FS);
	provinceManager.buildProvinceWeights(regionManager);
	provinceManager.buildPopRatios({}, false);

	// shove provinces into a chunk.
	V3::Chunk chunk;
	chunk.addSourceProvinceData(*provinceManager.getProvince(2), 27);
	chunk.addSourceProvinceData(*provinceManager.getProvince(3), 3);

	return chunk;
}
} // namespace

TEST(V3World_ChunkTests, SourceProvinceDataCanBeAdded)
{
	const auto chunk = prepChunk();

	const auto& spdata = chunk.getSourceProvinceData();
	ASSERT_EQ(2, spdata.size());
	const auto& [sp1, sp1weight] = spdata[0];
	const auto& [sp2, sp2weight] = spdata[1];

	EXPECT_EQ("TA2", sp1.owner);
	EXPECT_DOUBLE_EQ(27, sp1.weight);
	ASSERT_EQ(1, sp1.popRatios.size());
	EXPECT_EQ("culture", sp1.popRatios[0].getCulture());
	EXPECT_EQ("religion", sp1.popRatios[0].getReligion());
	EXPECT_THAT(sp1.eu4Capitals, testing::UnorderedElementsAre("TA2"));
	EXPECT_DOUBLE_EQ(27, sp1weight);

	EXPECT_EQ("TA3", sp2.owner);
	EXPECT_DOUBLE_EQ(3, sp2.weight);
	ASSERT_EQ(1, sp2.popRatios.size());
	EXPECT_EQ("culture2", sp2.popRatios[0].getCulture());
	EXPECT_EQ("religion2", sp2.popRatios[0].getReligion());
	EXPECT_THAT(sp2.eu4Capitals, testing::UnorderedElementsAre("TA3"));
	EXPECT_DOUBLE_EQ(3, sp2weight);
}

TEST(V3World_ChunkTests, CapitalsCanBeReported)
{
	const auto chunk = prepChunk();

	EXPECT_THAT(chunk.getKnownCapitals(), testing::UnorderedElementsAre("TA2", "TA3"));
}

TEST(V3World_ChunkTests, OwnerWeightsCanBeCalced)
{
	const auto chunk = prepChunk();

	const auto ownerWeights = chunk.calcOwnerWeights();

	EXPECT_THAT(ownerWeights, testing::UnorderedElementsAre(std::pair("TA2", 27), std::pair("TA3", 3)));
}

TEST(V3World_ChunkTests, TotalSourceProvinceWeightCanBeCalced)
{
	const auto chunk = prepChunk();

	// sum all all weight 27 + 3 = 30, divided by number of sources (2) = 15
	const auto totalWeight = chunk.getTotalSourceProvinceWeight();

	EXPECT_DOUBLE_EQ(15, totalWeight);
}

TEST(V3World_ChunkTests, ChunkCanDetectSourceSeaProvinces)
{
	// make a sea province
	auto sea = EU4::Province();
	sea.setSea();

	// shove province into a chunk.
	V3::Chunk chunk;
	chunk.addSourceProvinceData(sea, 1);

	EXPECT_TRUE(chunk.isSea());
}
