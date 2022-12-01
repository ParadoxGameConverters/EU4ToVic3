#include "SubStatePopEntries.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "PopEntry.h"

void V3::SubStatePopEntries::loadPops(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::SubStatePopEntries::registerKeys()
{
	registerKeyword("create_pop", [this](std::istream& theStream) {
		PopEntry popEntry;
		popEntry.loadPop(theStream);
		pops.push_back(popEntry.getPop());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
