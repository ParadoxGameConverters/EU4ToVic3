#include "Market.h"

#include <numeric>
#include <ranges>

V3::Market::Market(const std::vector<std::string>& possibleGoods)
{
	for (const auto& good: possibleGoods)
	{
		sellOrders[good] = 0;
		buyOrdersBuildings[good] = 0;
		buyOrdersPops[good] = 0;
	}
}

std::map<std::string, double> V3::Market::getMarketBalance() const
{
	std::map<std::string, double> marketBalance;
	for (const auto& [good, amount]: sellOrders)
	{
		marketBalance[good] = amount - buyOrdersBuildings.at(good) - buyOrdersPops.at(good);
	}

	return marketBalance;
}

std::map<std::string, double> V3::Market::getMarketShare(const std::vector<std::string>& goods) const
{
	double totalAdjustedOrders = 0;
	std::map<std::string, double> adjustedOrders;

	for (const std::string& good: goods)
	{
		const double adjustedOrder = sellOrders.at(good) - 0.5 * buyOrdersBuildings.at(good);
		totalAdjustedOrders += adjustedOrder;
		adjustedOrders[good] = adjustedOrder;
	}

	std::map<std::string, double> marketShare;
	for (const auto& [good, amount]: adjustedOrders)
	{
		marketShare[good] = amount / std::max(totalAdjustedOrders, 1.0);
	}

	return marketShare;
}

int V3::Market::estimateWealth(const std::string& strata)
{
	if (strata == "poor")
	{
		return 7;
	}
	if (strata == "middle")
	{
		return 12;
	}
	if (strata == "rich")
	{
		return 30;
	}

	Log(LogLevel::Warning) << "Unrecognized wealth strata: " << strata << ".";
	return 7;
}

std::set<std::string> V3::Market::getObsessions(const std::string& culture, const std::map<std::string, mappers::CultureDef>& cultures)
{
	// TODO(Gawquon): Read in obsessions to cultures
	std::set<std::string> obsessions;
	if (culture == "french")
		return std::set<std::string>{"wine"};
	return std::set<std::string>{};

	// return cultures.at(culture).obsessions;
}

std::set<std::string> V3::Market::getTaboos(const std::string& culture,
	 const std::map<std::string, mappers::CultureDef>& cultures,
	 const std::map<std::string, mappers::ReligionDef>& religions)
{
	return religions.at(cultures.at(culture).religion).taboos;
}

std::vector<std::string> V3::Market::enumerateGoods(const std::map<std::string, GoodsFulfillment>& map)
{
	std::vector<std::string> goods;
	for (const auto& good: map | std::views::keys)
	{
		goods.push_back(good);
	}
	return goods;
}

std::map<std::string, double> V3::Market::initCulturalFactors(const std::map<std::string, Good>& goodsMap)
{
	std::map<std::string, double> culturalFactors;
	for (const auto& good: goodsMap | std::views::keys)
	{
		culturalFactors[good] = 0;
	}
	return culturalFactors;
}

std::map<std::string, double> V3::Market::estimateCulturalPrevalence(const std::map<std::string, double>& cultureData,
	 const std::map<std::string, mappers::CultureDef>& cultures,
	 const std::map<std::string, mappers::ReligionDef>& religions,
	 const std::map<std::string, Good>& goodsMap)
{
	auto culturalFactors = initCulturalFactors(goodsMap);

	for (const auto& [culture, percent]: cultureData)
	{
		const auto& taboos = getTaboos(culture, cultures, religions);
		const auto& obsessions = getObsessions(culture, cultures);

		for (const auto& taboo: taboos)
		{
			culturalFactors.at(taboo) -= percent;
		}
		for (const auto& obsession: obsessions)
		{
			culturalFactors.at(obsession) += percent;
		}
	}

	return culturalFactors;
}

double V3::Market::calcCulturalFactor(const double culturalPrevalence)
{
	// 76% Obsession level(0.76 prevalence) -> 1.76x modifier
	// 92% Taboo level(-0.92 prevalence) -> 0.54x modifier;
	if (culturalPrevalence > 0)
		return culturalPrevalence + 1;
	if (culturalPrevalence < 0)
		return culturalPrevalence / 2 + 1;
	return 1;
}

double V3::Market::calcPopFactor(const double size,
	 const PopType& popType,
	 const Vic3DefinesLoader& defines,
	 const std::set<std::string>& laws,
	 const std::map<std::string, Law>& lawsMap)
{
	double workingRatio = popType.getDependentRatio().value_or(defines.getWorkingAdultRatioBase());
	workingRatio += calcAddedWorkingPopPercent(laws, lawsMap); // Propertied Woman
	return (size * workingRatio + size * (1 - workingRatio) * defines.getDependentConsumptionRatio()) * popType.getConsumptionRate() / 10000;
}

std::map<std::string, double> V3::Market::calcPurchaseWeights(const std::map<std::string, double>& marketShareMap,
	 const std::map<std::string, GoodsFulfillment>& fulfillments,
	 const std::map<std::string, double>& culturalPrevalence)
{
	double total = 0;
	std::map<std::string, double> purchaseWeights;

	for (const auto& goodName: marketShareMap | std::views::keys)
	{
		purchaseWeights[goodName] = calcPurchaseWeight(goodName, marketShareMap.at(goodName), fulfillments.at(goodName), culturalPrevalence.at(goodName));
		total += purchaseWeights.at(goodName);
	}

	for (const auto& goodName: marketShareMap | std::views::keys)
	{
		purchaseWeights.at(goodName) /= total == 0 ? 1.0 : total;
	}

	return purchaseWeights;
}

double V3::Market::calcPurchaseWeight(const std::string& goodName,
	 const double marketShare,
	 const GoodsFulfillment& fulfillment,
	 const double culturalPrevalence)
{
	double marketShareClamp = std::min(marketShare, fulfillment.maxShare);
	marketShareClamp = std::max(marketShareClamp, fulfillment.minShare);

	// Obsessions set minimum weight at 1.
	// Note(Gawquon): Because we already averaged out the effects of taboos/obsessions,
	// this minimum weight will be slightly inaccurate in populations
	// with a high prevalence of both the taboo and the obsession for  a given good.

	// Ex: A France could have 50% French pops(obsession Wine) and a 50% Muslim pops(taboo Wine).
	// We'd underestimate the Wine demand from the increase in minimum weights,
	// even though the net neutral multiplier would be accurate.

	double weight = fulfillment.weight;
	const double culturalFactor = calcCulturalFactor(culturalPrevalence);
	if (culturalFactor > 1)
	{
		const double percent = culturalFactor - 1;
		weight = std::max(weight, percent + (1 - percent) * weight); // (x * 1) + (1 - x)y
	}

	return marketShare * weight * culturalFactor;
}

double V3::Market::calcCulturalNeedFactor(const std::vector<std::string>& goods, const std::map<std::string, double>& culturalPrevalence)
{
	// Taboos tap out at x0.5, Obsessions at x2.0. But we're converting from prevalence(-1 <-> 1) into the +-25% bonus/malus to the base need.
	double culturalNeedFactor = 0;
	for (const std::string& goodName: goods)
	{
		culturalNeedFactor += culturalPrevalence.at(goodName) / 4;
	}

	return culturalNeedFactor + 1;
}

double V3::Market::calcAddedWorkingPopPercent(const std::set<std::string>& laws, const std::map<std::string, Law>& lawsMap)
{
	return std::accumulate(laws.begin(), laws.end(), 0.0, [lawsMap](double sum, const std::string& law) {
		return sum + lawsMap.at(law).workingAdultRatioAdd;
	});
}

void V3::Market::calcPopOrders(const int popSize,
	 const std::map<std::string, double>& jobData,
	 const std::map<std::string, double>& cultureData,
	 const Vic3DefinesLoader& defines,
	 const DemandLoader& demand,
	 const std::map<std::string, PopType>& popTypeMap,
	 const std::map<std::string, mappers::CultureDef>& cultures,
	 const std::map<std::string, mappers::ReligionDef>& religions,
	 const std::set<std::string>& laws,
	 const std::map<std::string, Law>& lawsMap)
{
	for (auto& value: buyOrdersPops | std::views::values)
	{
		value = 0;
	}

	const auto& culturalPrevalence = estimateCulturalPrevalence(cultureData, cultures, religions, demand.getGoodsMap());

	// Assuming enough land for each pop (for now).
	for (const auto& [job, jobPercent]: jobData)
	{
		// Validate popType.
		if (!popTypeMap.contains(job))
		{
			Log(LogLevel::Warning) << "PopType: " << job << " has no recognized game data.";
		}
		const auto& popType = popTypeMap.at(job);

		const double popFactor = calcPopFactor(popSize * jobPercent, popType, defines, laws, lawsMap);
		const int wealth = estimateWealth(popType.getStrata());

		// TODO(Gawquon) validate maps
		const auto& buyPackage = demand.getWealthConsumptionMap().at(wealth);

		for (const auto& [need, magnitude]: buyPackage.getPopneeds())
		{
			const auto& popneed = demand.getPopneedsMap().at(need);
			const auto& packageGoods = enumerateGoods(popneed.getGoodsFulfillment());

			const auto& marketShareMap = getMarketShare(packageGoods);
			const auto& purchaseWeights = calcPurchaseWeights(marketShareMap, popneed.getGoodsFulfillment(), culturalPrevalence);
			const double culturalNeedFactor = calcCulturalNeedFactor(packageGoods, culturalPrevalence);

			// Calc Pop demand
			for (const auto& goodName: packageGoods)
			{
				const double basePrice = demand.getGoodsMap().at(goodName).getBasePrice();
				const double popDemand = magnitude * culturalNeedFactor / basePrice * purchaseWeights.at(goodName) * popFactor;
				buyForPop(goodName, popDemand);
			}
			// TODO account for local goods quirks
		}
	}
}