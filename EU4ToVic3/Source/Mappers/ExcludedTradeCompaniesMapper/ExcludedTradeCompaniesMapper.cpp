#include "ExcludedTradeCompaniesMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::ExcludedTradeCompaniesMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading Excluded Trade Company Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << excludedTCs.size() << " excluded TCs loaded.";
}

void mappers::ExcludedTradeCompaniesMapper::registerKeys()
{
	registerKeyword("excluded_trade_companies", [this](std::istream& theStream) {
		auto names = commonItems::getStrings(theStream);
		excludedTCs.insert(names.begin(), names.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
