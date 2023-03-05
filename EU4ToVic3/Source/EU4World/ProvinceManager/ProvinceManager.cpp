#include "ProvinceManager.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <cmath>
#include <ranges>

void EU4::ProvinceManager::loadProvinces(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ProvinceManager::registerKeys()
{
	registerRegex(commonItems::integerRegex, [this](const std::string& numberString, std::istream& theStream) {
		auto newProvince = std::make_shared<Province>(numberString, theStream);
		provinces.emplace(newProvince->getID(), std::move(newProvince));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::shared_ptr<EU4::Province> EU4::ProvinceManager::getProvince(const int provinceID) const
{
	if (const auto& province = provinces.find(provinceID); province != provinces.end())
		return province->second;

	Log(LogLevel::Warning) << "Accessing EU4 province " << provinceID << " that does not exist (Save/EU4 version mismatch?)";
	return nullptr;
}

void EU4::ProvinceManager::loadParsers(const commonItems::ModFilesystem& modFS)
{
	defaultMapParser.loadDefaultMap(modFS);
	definitionScraper.loadDefinitions(modFS);
	buildingCostLoader.loadBuildingCosts(modFS);
}

void EU4::ProvinceManager::classifyProvinces(const RegionManager& regionManager)
{
	// We are poking at mapdata to classify provinces as land, sea, lake, rnw or wasteland.
	// Then we'll be discarding lakes, rnws and wastelands.
	// We'll also be assigning assimilation factors for popbuilding later.

	std::map<int, std::shared_ptr<Province>> viableProvinces;

	for (const auto& [provinceID, province]: provinces)
	{
		if (!definitionScraper.isValidID(provinceID))
		{
			// This means save contains province(s) not present in definitions. We're running against old or broken EU4 installation.
			// Continuing is not advised.
			Log(LogLevel::Error) << "Province " << provinceID << " was not found in map/definition.csv.";
			Log(LogLevel::Error) << "You are converting a newer save against an old EU4 installation, or you have mods that alter the map in a broken fashion.";
			Log(LogLevel::Error) << "Upgrade installation or fix the mod map definition errors to continue.";
			throw std::runtime_error("Unable to continue. Province inconsistency detected.");
		}

		if (defaultMapParser.isLake(provinceID)) // discard lake
		{
			discardedProvinces.emplace(provinceID, province);
			continue;
		}
		if (defaultMapParser.isRNW(provinceID)) // discard RNW
		{
			discardedProvinces.emplace(provinceID, province);
			continue;
		}
		if (!regionManager.provinceIsValid(provinceID)) // regionManager considers wastelands invalid, as they aren't registered.
		{
			// this is a special case. Wastelands DO map to something concrete - they generate their own chunks, albeit with no incoming demographics or ownership.
			// Yet they don't participate in geopolitical structure (regions) and need to be tiptoed around.

			// Mark as wasteland but don't discard.
			wastelands.emplace(provinceID, province);
			viableProvinces.emplace(provinceID, province);
			continue;
		}
		if (defaultMapParser.isSea(provinceID))
		{
			province->setSea();
		}
		else
		{
			// Whatever remains is a legit land province
			const auto& assimilationFactor = regionManager.getAssimilationFactor(provinceID);
			if (assimilationFactor)
				province->setAssimilationFactor(*assimilationFactor);
		}
		viableProvinces.emplace(provinceID, province);
	}
	provinces.swap(viableProvinces);
	Log(LogLevel::Info) << "<> Provinces filtered, " << provinces.size() << " remain.";
}

void EU4::ProvinceManager::buildPopRatios(const DatingData& datingData, bool convertAll)
{
	for (const auto& province: provinces | std::views::values)
		if (!province->isSea())
		{
			if (convertAll)
				province->purgeHistories();
			province->buildPopRatios(datingData);
		}
}

void EU4::ProvinceManager::buildProvinceWeights(const RegionManager& regionManager)
{
	std::map<std::string, double> superRegionInvestmentWeight;
	std::map<std::string, double> superRegionStartingWeight;
	std::map<std::string, double> superRegionProvinces;

	for (const auto& [provinceID, province]: provinces)
	{
		if (!regionManager.provinceIsValid(provinceID))
			continue;
		if (province->isSea())
			continue;
		province->determineProvinceWeight(buildingCostLoader);
		const auto& superRegion = regionManager.getParentSuperRegionName(provinceID);
		if (!superRegion)
			continue;
		superRegionInvestmentWeight[*superRegion] += province->getInvestedWeight();
		superRegionStartingWeight[*superRegion] += province->getProvinceHistory().getOriginalDevelopment();
		superRegionProvinces[*superRegion]++;
	}

	double europe = 0;
	for (const auto& [provinceID, province]: provinces)
	{
		if (!regionManager.provinceIsValid(provinceID))
			continue;
		if (province->isSea())
			continue;
		const auto& superRegion = regionManager.getParentSuperRegionName(provinceID);
		if (!superRegion)
			continue;
		const auto srWeightIncrease = superRegionInvestmentWeight[*superRegion];
		if (srWeightIncrease <= 0)
			continue;
		const auto srAverageWeightIncrease = srWeightIncrease / superRegionProvinces[*superRegion];
		const auto provinceIncreaseFactor = province->getInvestedWeight() / srAverageWeightIncrease;

		// This formula gives +0.5 for 5x over average increase of dev, and -0.25 max penalty for no invested dev.
		const auto investmentFactor = 1 / (1 + 3.32 * exp(-1.2 * provinceIncreaseFactor)) - 0.5;
		province->setInvestmentFactor(investmentFactor);
	}
}
