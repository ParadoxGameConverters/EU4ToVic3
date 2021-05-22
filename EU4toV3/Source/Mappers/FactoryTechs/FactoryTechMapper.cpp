#include "FactoryTechMapper.h"
#include "Configuration.h"
#include "FactoryTechDetails.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::FactoryTechMapper::FactoryTechMapper()
{
	LOG(LogLevel::Info) << "Parsing V2 Factory Techs";
	registerKeys();
	for (const auto& filename: commonItems::GetAllFilesInFolder(theConfiguration.getVic2Path() + "/technologies/"))
		parseFile(theConfiguration.getVic2Path() + "/technologies/" + filename);
	clearRegisteredKeywords();
}

mappers::FactoryTechMapper::FactoryTechMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::FactoryTechMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& tech, std::istream& theStream) {
		const FactoryTechDetails techDetails(theStream);
		// Every V2 technology activates but a single building, no worry of missing something.
		if (!techDetails.getFactoryName().empty())
			factoryTechMap.insert(std::make_pair(techDetails.getFactoryName(), tech));
	});
}

std::optional<std::string> mappers::FactoryTechMapper::getTechForFactoryType(const std::string& factoryType) const
{
	if (const auto& typeItr = factoryTechMap.find(factoryType); typeItr != factoryTechMap.end())
		return typeItr->second;
	else
		return std::nullopt;
}
