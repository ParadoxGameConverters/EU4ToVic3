#include "DemandLoader.h"
#include <ranges>

void V3::DemandLoader::loadGoods(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("common/goods"))
	{
		if (fileName.extension() != ".txt")
			continue;
		parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::DemandLoader::loadGoods(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::DemandLoader::loadPopNeeds(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("common/pop_needs"))
	{
		if (fileName.extension() != ".txt")
			continue;
		parseFile(fileName);
	}
	clearRegisteredKeywords();

	cacheGoodsNeedsMap();
}

void V3::DemandLoader::loadPopNeeds(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	cacheGoodsNeedsMap();
}

void V3::DemandLoader::loadBuyPackages(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("common/buy_packages"))
	{
		if (fileName.extension() != ".txt")
			continue;
		parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::DemandLoader::loadBuyPackages(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::DemandLoader::registerKeys()
{
	registerRegex("^(?!popneed_|wealth_)\\w+", [this](const std::string& goodName, std::istream& theStream) {
		auto good = Good(theStream);
		good.setName(goodName);
		goodsMap.emplace(goodName, good);
	});
	registerRegex("popneed_.*", [this](const std::string& popneed, std::istream& theStream) {
		popneedsMap.emplace(popneed, PopNeed(theStream));
	});
	registerRegex("wealth_.*", [this](const std::string& wealthLevel, std::istream& theStream) {
		try
		{
			int wealth = std::stoi(wealthLevel.substr(wealthLevel.find('_') + 1, wealthLevel.length()));
			wealthConsumptionMap.emplace(wealth, BuyPackage(theStream));
		}
		catch (...)
		{
			Log(LogLevel::Warning) << "Bad stoi in common/buy_packages/. wealth_(##)";
		}
	});
	IgnoreUnregisteredItems();
}

void V3::DemandLoader::cacheGoodsNeedsMap()
{
	for (const auto& [popneed_name, popneed]: popneedsMap)
	{
		for (const auto& good: popneed.getGoodsFulfillment() | std::views::keys)
		{
			if (const auto& it = goodsNeedsMap.find(good); it != goodsNeedsMap.end())
			{
				it->second.emplace(popneed_name);
			}
			goodsNeedsMap.emplace(good, std::set{popneed_name});
		}
	}
}