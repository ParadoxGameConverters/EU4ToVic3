#include "IdeaEffects/IdeaEffectMapper.h"
#include "gtest/gtest.h"

TEST(Mappers_IdeaEffectMapperTests, getEnforceFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getEnforceFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getEnforceFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getEnforceFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getEnforceFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tenforce = absolute_monarchy";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ("absolute_monarchy", theMapper.getEnforceFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getArmyFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getArmyFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getArmyFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getArmyFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getArmyFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tarmy = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getArmyFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getNavyFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getNavyFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getNavyFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getNavyFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getNavyFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tnavy = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getNavyFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getCommerceFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getCommerceFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getCommerceFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getCommerceFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getCommerceFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tcommerce = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getCommerceFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getCultureFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getCultureFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getCultureFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getCultureFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getCultureFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tculture = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getCultureFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getIndustryFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getIndustryFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getIndustryFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getIndustryFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getIndustryFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tindustry = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getIndustryFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getLibertyFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getLibertyFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getLibertyFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getLibertyFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getLibertyFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tliberty = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getLibertyFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getEqualityFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getEqualityFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getEqualityFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getEqualityFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getEqualityFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tequality = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getEqualityFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getOrderFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getOrderFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getOrderFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getOrderFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getOrderFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\torder = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getOrderFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getLiteracyFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getLiteracyFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getLiteracyFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getLiteracyFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getLiteracyFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tliteracy = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getLiteracyFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getLiberalFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getLiberalFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getLiberalFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getLiberalFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getLiberalFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tliberal = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getLiberalFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getReactionaryFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getReactionaryFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getReactionaryFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getReactionaryFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getReactionaryFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\treactionary = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getReactionaryFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getSlaveryFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getSlaveryFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getSlaveryFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getSlaveryFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getSlaveryFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tslavery = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getSlaveryFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getUpper_house_compositionFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getUpper_house_compositionFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getUpper_house_compositionFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getUpper_house_compositionFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getUpper_house_compositionFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tupper_house_composition = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getUpper_house_compositionFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getVote_franchiseFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getVote_franchiseFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getVote_franchiseFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getVote_franchiseFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getVote_franchiseFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tvote_franchise = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getVote_franchiseFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getVoting_systemFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getVoting_systemFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getVoting_systemFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getVoting_systemFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getVoting_systemFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tvoting_system = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getVoting_systemFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getPublic_meetingsFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getPublic_meetingsFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getPublic_meetingsFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getPublic_meetingsFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getPublic_meetingsFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tpublic_meetings = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getPublic_meetingsFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getPress_rightsFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getPress_rightsFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getPress_rightsFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getPress_rightsFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getPress_rightsFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tpress_rights = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getPress_rightsFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getTrade_unionsFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getTrade_unionsFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getTrade_unionsFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getTrade_unionsFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getTrade_unionsFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\ttrade_unions = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getTrade_unionsFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getPolitical_partiesFromIdeaReturnsNullOptForMissingIdea)
{
	std::stringstream input;
	const mappers::IdeaEffectMapper theMapper(input);

	const auto investment = theMapper.getPolitical_partiesFromIdea("missingIdea");
	ASSERT_EQ(std::nullopt, investment);
}

TEST(Mappers_IdeaEffectMapperTests, getPolitical_partiesFromIdeaDefaultsToNullopt)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(std::nullopt, theMapper.getPolitical_partiesFromIdea("specifiedIdea"));
}

TEST(Mappers_IdeaEffectMapperTests, getPolitical_partiesFromIdeaCanBeSetForIdea)
{
	std::stringstream input;
	input << "specifiedIdea ={\n";
	input << "\tpolitical_parties = 1";
	input << "}";
	const mappers::IdeaEffectMapper theMapper(input);

	ASSERT_EQ(-4, theMapper.getPolitical_partiesFromIdea("specifiedIdea"));
}
