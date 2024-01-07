#ifndef RELIGION_DEFINITION_LOADER_H
#define RELIGION_DEFINITION_LOADER_H
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"
#include "ReligionDef.h"

namespace mappers
{
class ReligionDefinitionLoader: commonItems::parser
{
  public:
	ReligionDefinitionLoader() = default;
	void loadDefinitions(const commonItems::ModFilesystem& modFS);
	void loadDefinitions(std::istream& theStream);

	[[nodiscard]] std::optional<ReligionDef> getReligionDef(const std::string& religion) const;
	[[nodiscard]] const auto& getReligionDefs() const { return religionDefinitions; }

  private:
	void registerKeys();

	std::map<std::string, ReligionDef> religionDefinitions;
};
} // namespace mappers

#endif // RELIGION_DEFINITION_LOADER_H
