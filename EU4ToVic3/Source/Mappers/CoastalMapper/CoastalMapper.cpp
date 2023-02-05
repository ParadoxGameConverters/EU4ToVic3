#include "CoastalMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::CoastalMapper::loadAdjacencies(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading Physical Province Adjacencies.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << adjacencies.size() << " adjacencies.";
}

void mappers::CoastalMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& sourceProvince, std::istream& theStream) {
		auto targets = commonItems::getStrings(theStream);
		adjacencies.emplace(sourceProvince, std::set<std::string>());
		adjacencies.at(sourceProvince).insert(targets.begin(), targets.end());
	});
}

std::set<std::string> mappers::CoastalMapper::getAdjacencies(const std::string& province) const
{
	if (adjacencies.contains(province))
		return adjacencies.at(province);
	return std::set<std::string>{};
}
