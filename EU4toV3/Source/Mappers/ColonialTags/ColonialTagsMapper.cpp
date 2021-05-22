#include "ColonialTagsMapper.h"
#include "Parser.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::ColonialTagMapper::ColonialTagMapper()
{
	registerKeys();
	parseFile("configurables/colonial_tags.txt");
	clearRegisteredKeywords();
}

mappers::ColonialTagMapper::ColonialTagMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ColonialTagMapper::registerKeys()
{
	registerKeyword("link", [this](const std::string& unused, std::istream& theStream) {
		const ColonialTag colonialBlock(theStream);
		colonyList.push_back(colonialBlock.getColonyTag());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ColonialTagMapper::getColonialTag(const std::string& eu4Region, const std::string& v2Region, const std::string& cultureGroup) const
{
	for (const auto& colonyTag: colonyList)
		if (colonyTag.match(eu4Region, v2Region, cultureGroup))
			return colonyTag.tag;
	return std::nullopt;
}
