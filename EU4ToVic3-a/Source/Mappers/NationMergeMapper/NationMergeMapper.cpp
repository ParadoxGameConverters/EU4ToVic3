#include "NationMergeMapper.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"

void mappers::NationMergeMapper::loadNationMerge()
{
	registerKeys();
	parseFile("configurables/merge_nations.txt");
	clearRegisteredKeywords();
}

void mappers::NationMergeMapper::loadNationMerge(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::NationMergeMapper::registerKeys()
{
	registerKeyword("merge_daimyos", [this](std::istream& theStream) {
		mergeDaimyos = commonItems::getString(theStream) == "yes";
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& unused, std::istream& theStream) {
		const MergeBlock newBlock(theStream);
		mergeBlocks.push_back(newBlock);
	});
}
