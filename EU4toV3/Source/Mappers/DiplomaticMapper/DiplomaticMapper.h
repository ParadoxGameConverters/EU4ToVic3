#ifndef DIPLOMATIC_MAPPER_H
#define DIPLOMATIC_MAPPER_H
#include "Parser.h"
#include <set>

namespace mappers
{
class DiplomaticMapper: commonItems::parser
{
  public:
	DiplomaticMapper() = default;
	void loadMappingRules(const std::string& filePath);

	[[nodiscard]] auto isAgreementInDominions(const std::string& agreement) const { return dominions.contains(agreement); }
	[[nodiscard]] auto isAgreementInProtectorates(const std::string& agreement) const { return protectorates.contains(agreement); }
	[[nodiscard]] auto isAgreementInDefensivePacts(const std::string& agreement) const { return defensivePacts.contains(agreement); }
	[[nodiscard]] auto isAgreementInTributaries(const std::string& agreement) const { return tributaries.contains(agreement); }
	[[nodiscard]] auto isAgreementInPersonalUnions(const std::string& agreement) const { return personalUnions.contains(agreement); }
	[[nodiscard]] auto isAgreementInPuppets(const std::string& agreement) const { return puppets.contains(agreement); }
	[[nodiscard]] auto isAgreementInVassals(const std::string& agreement) const { return vassals.contains(agreement); }
	[[nodiscard]] auto isAgreementInDoubleRelationshipBoosts(const std::string& agreement) const { return doubleRelationshipBoosts.contains(agreement); }
	[[nodiscard]] auto isAgreementInDoubleDefensivePacts(const std::string& agreement) const { return doubleDefensivePacts.contains(agreement); }

  private:
	void registerKeys();

	std::set<std::string> dominions;
	std::set<std::string> protectorates;
	std::set<std::string> defensivePacts;
	std::set<std::string> tributaries;
	std::set<std::string> personalUnions;
	std::set<std::string> puppets;
	std::set<std::string> vassals;
	std::set<std::string> doubleRelationshipBoosts;
	std::set<std::string> doubleDefensivePacts;
};
} // namespace mappers

#endif // DIPLOMATIC_MAPPER_H
