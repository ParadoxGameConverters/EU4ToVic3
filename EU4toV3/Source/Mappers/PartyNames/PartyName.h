#ifndef PARTY_NAME_H
#define PARTY_NAME_H
#include "Parser.h"

namespace mappers
{
class PartyName: commonItems::parser
{
  public:
	explicit PartyName(std::istream& theStream);

	[[nodiscard]] const auto& getLanguageToNameMap() const { return languageToNameMap; }

  private:
	void registerKeys();

	std::map<std::string, std::string> languageToNameMap;
};
} // namespace mappers

#endif // PARTY_NAME_H