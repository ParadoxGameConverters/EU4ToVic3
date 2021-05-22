#ifndef IDEA_EFFECT_MAPPER_H
#define IDEA_EFFECT_MAPPER_H
#include "IdeaEffects.h"
#include "Parser.h"

namespace mappers
{
class IdeaEffectMapper: commonItems::parser
{
  public:
	IdeaEffectMapper();
	explicit IdeaEffectMapper(std::istream& theStream);

	[[nodiscard]] std::optional<std::string> getEnforceFromIdea(const std::string& ideaName) const;

	[[nodiscard]] std::optional<int> getArmyFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getCommerceFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getCultureFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getIndustryFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getNavyFromIdea(const std::string& ideaName) const;

	[[nodiscard]] std::optional<int> getSlaveryFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getUpper_house_compositionFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getVote_franchiseFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getVoting_systemFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getPublic_meetingsFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getPress_rightsFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getTrade_unionsFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getPolitical_partiesFromIdea(const std::string& ideaName) const;

	[[nodiscard]] std::optional<int> getLibertyFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getEqualityFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getOrderFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getLiteracyFromIdea(const std::string& ideaName) const;

	[[nodiscard]] std::optional<int> getReactionaryFromIdea(const std::string& ideaName) const;
	[[nodiscard]] std::optional<int> getLiberalFromIdea(const std::string& ideaName) const;

  private:
	void registerKeys();

	std::map<std::string, IdeaEffects> ideas;
};
} // namespace mappers

#endif // IDEA_EFFECT_MAPPER_H
