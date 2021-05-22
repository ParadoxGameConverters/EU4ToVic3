#include "ColonialTags/ColonialTagsMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_ColonialTagsMapperTests, colonialTagMatchReturnsNulloptForNoCandidates)
{
	std::stringstream input;
	const mappers::ColonialTagMapper mapper(input);

	const auto& match = mapper.getColonialTag("eu4Region", "v2Region", "cultureGroup");
	
	ASSERT_EQ(std::nullopt, match);
}

TEST(Mappers_ColonialTagsMapperTests, colonialTagMatchReturnsFirstCandidate)
{
	std::stringstream input;
	input << "link = { tag = ORA v2region = ENG_2087 cultureGroup = beneluxian }\n";
	input << "link = { tag = TRN v2region = ENG_2087 cultureGroup = beneluxian }\n";
	const mappers::ColonialTagMapper mapper(input);

	const auto& match = mapper.getColonialTag("", "ENG_2087", "beneluxian");

	ASSERT_EQ("ORA", *match);
}
