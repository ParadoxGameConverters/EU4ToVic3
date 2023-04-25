#ifndef TRAIT_DEFINITION_LOADER_H
#define TRAIT_DEFINITION_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "TraitDef.h"

namespace mappers
{
class TraitDefinitionLoader: commonItems::parser
{
  public:
	TraitDefinitionLoader() = default;
	void loadDefinitions(const commonItems::ModFilesystem& modFS);
	void loadDefinitions(std::istream& theStream);

	[[nodiscard]] const auto& getDefinitions() const { return traitDefinitions; }

  private:
	void registerKeys();

	std::map<std::string, TraitDef> traitDefinitions;
};
} // namespace mappers

#endif // TRAIT_DEFINITION_LOADER_H
