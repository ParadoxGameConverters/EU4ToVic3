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

	[[nodiscard]] std::optional<std::string> getAgreementType(const std::string& agreement) const;
	[[nodiscard]] int getRelationshipBoost(const std::string& agreement) const;

  private:
	void registerKeys();

	std::map<int, std::set<std::string>> relationshipBoosts;
	std::map<std::string, std::set<std::string>> agreementTypes;
};
} // namespace mappers

#endif // DIPLOMATIC_MAPPER_H
