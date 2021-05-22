#include "FactoryInventionEffect.h"
#include "FactoryInventionDetails.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::FactoryInventionEffect::FactoryInventionEffect(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::FactoryInventionEffect::registerKeys()
{
	registerKeyword("effect", [this](const std::string& unused, std::istream& theStream) {
		const FactoryInventionDetails effectBlock(theStream);
		factoryName = effectBlock.getFactoryName();
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
