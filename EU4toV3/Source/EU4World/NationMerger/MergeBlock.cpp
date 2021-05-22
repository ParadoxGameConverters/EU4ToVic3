#include "MergeBlock.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

EU4::MergeBlock::MergeBlock(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::MergeBlock::registerKeys()
{
	registerSetter("master", master);
	registerKeyword("slave", [this](std::istream& theStream) {
		slaves.insert(commonItems::getString(theStream));
	});
	registerKeyword("merge", [this](std::istream& theStream) {
		merge = commonItems::getString(theStream) == "yes";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
