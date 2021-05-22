#include "PopMapper.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::PopMapper::PopMapper(const std::string& fileName)
{
	registerKeys();
	parseFile(fileName);
	clearRegisteredKeywords();
}

mappers::PopMapper::PopMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PopMapper::registerKeys()
{
	registerRegex(R"(\d+)", [this](const std::string& provinceID, std::istream& theStream) {
		const PopTypes newPopType(theStream);
		provincePops.insert(std::make_pair(stoi(provinceID), newPopType.getPopTypes()));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
