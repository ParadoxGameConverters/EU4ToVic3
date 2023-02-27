#ifndef RELIGION_DEFINITION_ENTRY_H
#define RELIGION_DEFINITION_ENTRY_H
#include "Parser.h"
#include "ReligionDef.h"

namespace mappers
{
class ReligionDefinitionEntry: commonItems::parser
{
  public:
	explicit ReligionDefinitionEntry(std::istream& theStream);

	[[nodiscard]] const auto& getReligionDef() const { return religionDef; }

  private:
	void registerkeys();

	ReligionDef religionDef;
};
} // namespace mappers

#endif // RELIGION_DEFINITION_ENTRY_H