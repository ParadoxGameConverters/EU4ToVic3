#include "GovernmentMapper.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::GovernmentMapper::GovernmentMapper()
{
	LOG(LogLevel::Info) << "Parsing governments mappings.";
	registerKeys();
	parseFile("configurables/government_mapping.txt");
	clearRegisteredKeywords();
}

mappers::GovernmentMapper::GovernmentMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::GovernmentMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& sourceGov, std::istream& theStream) {
		for (const auto& destGov: commonItems::stringList(theStream).getStrings())
			governmentMap.insert(make_pair(destGov, sourceGov));
	});
}

std::optional<std::string> mappers::GovernmentMapper::matchGovernment(const std::string& sourceGovernment) const
{

	if (const auto& mapping = governmentMap.find(sourceGovernment); mapping != governmentMap.end())
	{
		return mapping->second;
	}
	else
	{
		LOG(LogLevel::Warning) << "No government mapping defined for " << sourceGovernment << "!";
		return std::nullopt;
	}
}
