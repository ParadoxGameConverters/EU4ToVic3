#include "FactoryInventionMapper.h"
#include "Configuration.h"
#include "FactoryInventionEffect.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::FactoryInventionMapper::FactoryInventionMapper()
{
	LOG(LogLevel::Info) << "Parsing V2 Factory Inventions";
	registerKeys();
	for (const auto& filename: commonItems::GetAllFilesInFolder(theConfiguration.getVic2Path() + "/inventions/"))
		parseFile(theConfiguration.getVic2Path() + "/inventions/" + filename);
	clearRegisteredKeywords();
}

mappers::FactoryInventionMapper::FactoryInventionMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::FactoryInventionMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& invention, std::istream& theStream) {
		const FactoryInventionEffect theEffect(theStream);
		if (!theEffect.getFactoryName().empty())
			factoryInventionMap.insert(std::make_pair(theEffect.getFactoryName(), invention));
	});
}

std::optional<std::string> mappers::FactoryInventionMapper::getInventionForFactoryType(const std::string& factoryType) const
{
	if (const auto& typeItr = factoryInventionMap.find(factoryType); typeItr != factoryInventionMap.end())
		return typeItr->second;
	else
		return std::nullopt;
}
