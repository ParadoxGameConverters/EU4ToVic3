#include "StartingInventionMapper.h"
#include "StartingInventionLimit.h"
#include "ParserHelpers.h"
#include "OSCompatibilityLayer.h"
#include "../../Configuration.h"
#include "Log.h"
#include <set>

mappers::StartingInventionMapper::StartingInventionMapper()
{
	LOG(LogLevel::Info) << "Parsing Starting Inventions";
	registerKeys();
	auto filenames = commonItems::GetAllFilesInFolder(theConfiguration.getVic2Path() + "/inventions/");
	for (const auto& filename : filenames)
	{
		parseFile(theConfiguration.getVic2Path() + "/inventions/" + filename);
	}
	clearRegisteredKeywords();
}

mappers::StartingInventionMapper::StartingInventionMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::StartingInventionMapper::registerKeys()
{
	// we need èüéö for jean_jaurès, johann_heinrich_von_thünen, léon_walras, eugen_von_böhm_bawerk :/
	// If intellisense or something else complains, don't alter, it works.
	registerRegex("[èüéöa-z_]+", [this](const std::string& invention, std::istream& theStream)
		{
			const StartingInventionLimit theLimit(theStream);
			if (!theLimit.getTechName().empty())
			{
				inventionTechMap.insert(std::make_pair(invention, theLimit.getTechName()));
			}
		});
	registerRegex("[a-zA-Z0-9\\_.:]+", commonItems::ignoreItem);
}

[[nodiscard]] std::set<std::string> mappers::StartingInventionMapper::getInventionsForTechs(const std::set<std::string>& incTechs) const
{
	std::set<std::string> inventions;
	for (const auto& tech: incTechs)
	{
		for (const  auto& invention: inventionTechMap)
		{
			if (invention.second == tech) inventions.insert(invention.first);
		}		
	}
	return inventions;
}
