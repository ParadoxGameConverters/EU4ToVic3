#ifndef PARTY_TYPE_MAPPER_H
#define PARTY_TYPE_MAPPER_H
#include "Parser.h"
#include "PartyType.h"

namespace mappers
{
class PartyTypeMapper: commonItems::parser
{
  public:
	PartyTypeMapper();
	explicit PartyTypeMapper(std::istream& theStream);

	[[nodiscard]] std::optional<PartyType> getPartyTypeByIdeology(const std::string& ideology) const;

  private:
	void registerKeys();
	
	std::map<std::string, PartyType> partyTypeMap;
};
} // namespace mappers

#endif // PARTY_TYPE_MAPPER_H