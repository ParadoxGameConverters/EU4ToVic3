#ifndef CULTURE_DEFINITION_LOADER_H
#define CULTURE_DEFINITION_LOADER_H
#include "CultureDef.h"
#include "ModLoader/ModFilesystem.h"
#include "Parser.h"

namespace mappers
{
class CultureDefinitionLoader: commonItems::parser
{
  public:
	CultureDefinitionLoader() = default;
	void loadDefinitions(const commonItems::ModFilesystem& modFS);
	void loadDefinitions(std::istream& theStream);

	[[nodiscard]] const auto& getDefinitions() const { return cultureDefinitions; }

  private:
	void registerKeys();

	std::map<std::string, CultureDef> cultureDefinitions;
	bool skipProcessing = true; // skip processing for cultures not loaded from 99_<...>.txt!
	bool skipExport = true;		 // skip export for cultures loaded from */blankMod/*
};
} // namespace mappers

#endif // CULTURE_DEFINITION_LOADER_H
