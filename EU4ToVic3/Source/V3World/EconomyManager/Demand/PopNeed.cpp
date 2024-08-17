#include "PopNeed.h"

V3::PopNeed::PopNeed(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::PopNeed::registerKeys()
{
	goodsParser.registerKeyword("goods", [this](std::istream& theStream) {
		goodsFulfillmentBuilder.good = commonItems::getString(theStream);
	});
	goodsParser.registerKeyword("weight", [this](std::istream& theStream) {
		goodsFulfillmentBuilder.weight = commonItems::getDouble(theStream);
	});
	goodsParser.registerKeyword("max_supply_share", [this](std::istream& theStream) {
		goodsFulfillmentBuilder.maxShare = commonItems::getDouble(theStream);
	});
	goodsParser.registerKeyword("min_supply_share", [this](std::istream& theStream) {
		goodsFulfillmentBuilder.minShare = commonItems::getDouble(theStream);
	});
	goodsParser.IgnoreUnregisteredItems();

	registerKeyword("default", [this](std::istream& theStream) {
		defaultGood = commonItems::getString(theStream);
	});
	registerKeyword("entry", [this](std::istream& theStream) {
		goodsParser.parseStream(theStream);
		goodsFulfillment.emplace(goodsFulfillmentBuilder.good, goodsFulfillmentBuilder);
		goodsFulfillmentBuilder = {};
	});
}
