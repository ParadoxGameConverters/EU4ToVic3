#include "FactoryStartingCounts.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::FactoryStartingCounts::FactoryStartingCounts()
{
	LOG(LogLevel::Info) << "Parsing starting factories.";
	registerKeywords();
	parseFile("configurables/starting_factories.txt");
	clearRegisteredKeywords();
}

mappers::FactoryStartingCounts::FactoryStartingCounts(std::istream& theStream)
{
	registerKeywords();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::FactoryStartingCounts::registerKeywords()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& factoryName, std::istream& theStream) {
		startingCounts.insert(std::make_pair(factoryName, commonItems::singleInt(theStream).getInt()));
	});
}

std::optional<int> mappers::FactoryStartingCounts::getCountForFactoryType(const std::string& factoryType) const
{
	if (const auto& typeItr = startingCounts.find(factoryType); typeItr != startingCounts.end())
		return typeItr->second;
	else
		return std::nullopt;
}
