#ifndef CULTURE_DEFINITION_ENTRY_H
#define CULTURE_DEFINITION_ENTRY_H
#include "CultureDef.h"
#include "Parser.h"

namespace mappers
{
class CultureDefinitionEntry: commonItems::parser
{
  public:
	explicit CultureDefinitionEntry(std::istream& theStream);

	[[nodiscard]] const auto& getCultureDef() const { return cultureDef; }

  private:
	void registerkeys();
	parser ethStripper;

	CultureDef cultureDef;
};
} // namespace mappers

#endif // CULTURE_DEFINITION_ENTRY_H