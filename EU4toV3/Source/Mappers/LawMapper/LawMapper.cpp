#include "LawMapper.h"
#include "CommonRegexes.h"
#include "LawGroup.h"
#include "Log.h"

void mappers::LawMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading Law Mapping Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << lawGroups.size() << " law groups loaded.";
}

void mappers::LawMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::LawMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& lawGroupName, std::istream& theStream) {
		lawGroups.emplace(lawGroupName, LawGroup(theStream).getLaws());
	});
}
