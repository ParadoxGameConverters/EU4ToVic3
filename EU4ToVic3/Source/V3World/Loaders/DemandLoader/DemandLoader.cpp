#include "DemandLoader.h"

void V3::DemandLoader::loadPopNeeds(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/pop_needs/"))
	{
		if (getExtension(fileName) != "txt")
			continue;
		parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::DemandLoader::loadBuyPackages(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/buy_packages/"))
	{
		if (getExtension(fileName) != "txt")
			continue;
		parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::DemandLoader::registerKeys()
{
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