#ifndef IDEA_EFFECTS_H
#define IDEA_EFFECTS_H
#include "Parser.h"

namespace mappers
{
class IdeaEffects: commonItems::parser
{
  public:
	explicit IdeaEffects(std::istream& theStream);

	[[nodiscard]] const auto& getEnforce() const { return enforce; }

	[[nodiscard]] const auto& getArmy() const { return army; }
	[[nodiscard]] const auto& getNavy() const { return navy; }
	[[nodiscard]] const auto& getCommerce() const { return commerce; }
	[[nodiscard]] const auto& getCulture() const { return culture; }
	[[nodiscard]] const auto& getIndustry() const { return industry; }

	[[nodiscard]] const auto& getOrder() const { return order; }
	[[nodiscard]] const auto& getLiberty() const { return liberty; }
	[[nodiscard]] const auto& getEquality() const { return equality; }
	[[nodiscard]] const auto& getLiteracy() const { return literacy; }

	[[nodiscard]] const auto& getReactionary() const { return reactionary; }
	[[nodiscard]] const auto& getLiberal() const { return liberal; }

	[[nodiscard]] const auto& getSlavery() const { return slavery; }
	[[nodiscard]] const auto& getUpper_house_composition() const { return upper_house_composition; }
	[[nodiscard]] const auto& getVote_franchise() const { return vote_franchise; }
	[[nodiscard]] const auto& getVoting_system() const { return voting_system; }
	[[nodiscard]] const auto& getPublic_meetings() const { return public_meetings; }
	[[nodiscard]] const auto& getPress_rights() const { return press_rights; }
	[[nodiscard]] const auto& getTrade_unions() const { return trade_unions; }
	[[nodiscard]] const auto& getPolitical_parties() const { return political_parties; }

  private:
	void registerKeys();

	// Input values range 0-10. However, for set values we return [-5 to 5] because working in [0 to 10] makes no sense internally.
	std::optional<std::string> enforce;

	std::optional<int> slavery;
	std::optional<int> upper_house_composition;
	std::optional<int> vote_franchise;
	std::optional<int> voting_system;
	std::optional<int> public_meetings;
	std::optional<int> press_rights;
	std::optional<int> trade_unions;
	std::optional<int> political_parties;

	std::optional<int> liberty;
	std::optional<int> equality;
	std::optional<int> order;
	std::optional<int> literacy;

	std::optional<int> reactionary;
	std::optional<int> liberal;

	std::optional<int> army;
	std::optional<int> navy;
	std::optional<int> commerce;
	std::optional<int> culture;
	std::optional<int> industry;
};
} // namespace mappers

#endif // IDEA_EFFECTS_H