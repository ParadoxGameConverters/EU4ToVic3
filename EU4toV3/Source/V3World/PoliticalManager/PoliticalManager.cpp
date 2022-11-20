#include "PoliticalManager.h"
#include "CountryDefinitionLoader/CountryDefinitionLoader.h"
#include "Log.h"

void V3::PoliticalManager::initializeVanillaCountries(const std::string& V3Path)
{
	Log(LogLevel::Info) << "-> Loading Vanilla Countries.";

	CountryDefinitionLoader definitionLoader;
	definitionLoader.loadCommonCountries(V3Path);
	countries = definitionLoader.getCountries();

	Log(LogLevel::Info) << "<> " << countries.size() << " vanilla countries loaded.";
}
