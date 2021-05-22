#ifndef PARTY_NAME_MAPPER_H
#define PARTY_NAME_MAPPER_H
#include "Parser.h"
#include "PartyName.h"

namespace mappers
{
class PartyNameMapper: commonItems::parser
{
  public:
	PartyNameMapper();
	explicit PartyNameMapper(std::istream& theStream);

	[[nodiscard]] const auto& getPartyToLanguageMap() const { return partyToLanguageMap; }

  private:
	void registerKeys();
	std::map<std::string, PartyName> partyToLanguageMap;
};
} // namespace mappers

#endif // PARTY_NAME_MAPPER_H