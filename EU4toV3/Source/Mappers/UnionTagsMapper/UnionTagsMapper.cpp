#include "UnionTagsMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

mappers::UnionTagsMapper::UnionTagsMapper()
{
	LOG(LogLevel::Info) << "Parsing union tags.";
	registerKeys();
	parseFile("configurables/union_tags.txt");
	clearRegisteredKeywords();
}

mappers::UnionTagsMapper::UnionTagsMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::UnionTagsMapper::registerKeys()
{
	registerKeyword("union_tags", [this](const std::string& unused, std::istream& theStream) {
		auto theTags = commonItems::getStrings(theStream);
		unionTags = std::set(theTags.begin(), theTags.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
