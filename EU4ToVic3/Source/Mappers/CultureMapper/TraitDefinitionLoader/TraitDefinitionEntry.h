#ifndef TRAIT_DEFINITION_ENTRY_H
#define TRAIT_DEFINITION_ENTRY_H
#include "Parser.h"
#include "TraitDef.h"

namespace mappers
{
class TraitDefinitionEntry: commonItems::parser
{
  public:
	explicit TraitDefinitionEntry(std::istream& theStream);

	[[nodiscard]] const auto& getTraitDef() const { return traitDef; }

  private:
	void registerkeys();

	TraitDef traitDef;
};
} // namespace mappers

#endif // TRAIT_DEFINITION_ENTRY_H