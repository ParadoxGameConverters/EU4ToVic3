#include "NationMergeParser.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::NationMergeParser::NationMergeParser()
{
	registerKeys();
	parseFile("configurables/merge_nations.txt");
	clearRegisteredKeywords();
}

EU4::NationMergeParser::NationMergeParser(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::NationMergeParser::registerKeys()
{
	registerKeyword("merge_daimyos", [this](std::istream& theStream) {
		mergeDaimyos = commonItems::getString(theStream) == "yes";
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& unused, std::istream& theStream) {
		const MergeBlock newBlock(theStream);
		mergeBlocks.push_back(newBlock);
	});
}
