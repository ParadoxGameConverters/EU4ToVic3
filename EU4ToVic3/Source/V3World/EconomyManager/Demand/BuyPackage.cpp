#include "BuyPackage.h"

V3::BuyPackage::BuyPackage(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::BuyPackage::registerKeys()
{
	goodsParser.registerRegex("popneed_.*", [this](const std::string& popneed, std::istream& theStream) {
		popneedMap.emplace(popneed, commonItems::getInt(theStream));
	});
	goodsParser.IgnoreUnregisteredItems();

	registerKeyword("goods", [this](std::istream& theStream) {
		goodsParser.parseStream(theStream);
	});
	IgnoreUnregisteredItems();
}
