#include "Market.h"

#include <algorithm>
#include <iomanip>
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

void V3::Market::loadGoods(const std::map<std::string, Good>& goodsList)
{
	for (const auto& good: goodsList | std::views::keys)
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

// Returns the true market share, before any adjustments/clamps.
std::map<std::string, double> V3::Market::getMarketShare(const std::vector<std::string>& goods) const
{
	double totalAdjustedOrders = 0;
	std::map<std::string, double> adjustedOrders;

	for (const std::string& good: goods)
	{
		if (!validateGood(good))
		{
			continue;
		}

		const double adjustedOrder = std::max(sellOrders.at(good) - 0.5 * buyOrdersBuildings.at(good), 1.0);
		totalAdjustedOrders += adjustedOrder;
		adjustedOrders[good] = adjustedOrder;
	}

	std::map<std::string, double> marketShare;
	for (const auto& [good, amount]: adjustedOrders)
	{
		marketShare[good] = amount / totalAdjustedOrders;
	}

	return marketShare;
}

void V3::Market::sell(const std::string& good, const double amount)
{
	if (!validateGood(good))
	{
		return;
	}

	sellOrders.at(good) += amount;
}

void V3::Market::buyForBuilding(const std::string& good, double const amount)
{
	if (!validateGood(good))
	{
		return;
	}

	buyOrdersBuildings.at(good) += amount;
}

void V3::Market::buyForPop(const std::string& good, double const amount)
{
	if (!validateGood(good))
	{
		return;
	}

	buyOrdersPops.at(good) += amount;
}


int V3::Market::estimateWealth(const int startingQOL)
{
	if (startingQOL < 10)
	{
		return 8;
	}
	if (startingQOL < 20)
	{
		return 17;
	}
	if (startingQOL < 40)
	{
		return 30;
	}

	Log(LogLevel::Warning) << "Unexpected poptype starting QOL: " << startingQOL << ".";
	return 7;
}

std::set<std::string> V3::Market::getObsessions(const std::string& culture, const std::map<std::string, mappers::CultureDef>& cultures)
{
	if (!cultures.contains(culture))
	{
		if (!cultureErrors.contains(culture))
		{
			const auto& theCulture = culture.empty() ? "None" : culture;
			Log(LogLevel::Warning) << "Culture: " << theCulture << " has no definition. Assuming no obsessions or taboos.";
		}
		return {};
	}
	return cultures.at(culture).obsessions;
}

std::set<std::string> V3::Market::getTaboos(const std::string& culture,
	 const std::map<std::string, mappers::CultureDef>& cultures,
	 const std::map<std::string, mappers::ReligionDef>& religions)
{
	if (!cultures.contains(culture))
	{
		return {};
	}
	const auto religion = cultures.at(culture).religion;
	if (!religions.contains(religion))
	{
		if (!religionErrors.contains(religion))
		{
			const auto& theReligion = religion.empty() ? "None" : religion;
			Log(LogLevel::Warning) << "Religion: " << theReligion << " has no definition. Assuming no taboos.";
		}
		return {};
	}
	return religions.at(religion).taboos;
}

std::vector<std::string> V3::Market::enumerateGoods(const std::map<std::string, GoodsFulfillment>& map, const std::map<std::string, Good>& goodsMap) const
{
	std::vector<std::string> goods;
	for (const auto& good: map | std::views::keys)
	{
		if (!goodsMap.contains(good))
		{
			if (!goodsErrors.contains(good))
			{
				Log(LogLevel::Warning) << "Good: " << good << " has no definition.";
				goodsErrors.emplace(good);
			}
			continue;
		}
		if (validateGood(good))
		{
			goods.push_back(good);
		}
	}
	return goods;
}

std::map<std::string, double> V3::Market::initCulturalFactors() const
{
	std::map<std::string, double> culturalFactors;
	for (const auto& good: sellOrders | std::views::keys)
	{
		culturalFactors[good] = 0;
	}
	return culturalFactors;
}

std::map<std::string, double> V3::Market::estimateCulturalPrevalence(const std::map<std::string, double>& cultureData,
	 const std::map<std::string, mappers::CultureDef>& cultures,
	 const std::map<std::string, mappers::ReligionDef>& religions) const
{
	auto culturalFactors = initCulturalFactors();

	for (const auto& [culture, fraction]: cultureData)
	{
		const auto& taboos = getTaboos(culture, cultures, religions);
		const auto& obsessions = getObsessions(culture, cultures);

		for (const auto& taboo: taboos)
		{
			if (!validateGood(taboo))
			{
				continue;
			}
			culturalFactors.at(taboo) -= fraction;
		}
		for (const auto& obsession: obsessions)
		{
			if (!validateGood(obsession))
			{
				continue;
			}
			culturalFactors.at(obsession) += fraction;
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
	workingRatio += calcAddedWorkingPopFraction(laws, lawsMap); // Propertied Woman
	return (size * workingRatio + size * (1 - workingRatio) * defines.getDependentConsumptionRatio()) * popType.getConsumptionRate() / 10000;
}

std::map<std::string, double> V3::Market::calcPurchaseWeights(const std::map<std::string, double>& marketShareMap,
	 const std::map<std::string, GoodsFulfillment>& fulfillments,
	 const std::map<std::string, double>& culturalPrevalence,
	 const std::map<std::string, Good>& goodsMap)
{
	double total = 0;
	std::map<std::string, double> purchaseWeights;

	for (const auto& good: marketShareMap | std::views::keys)
	{
		// Local goods assumed to have a 1/n market share, this will result in overestimation, but will subsequently be clamped by any max_share
		double marketShare = marketShareMap.at(good);
		if (goodsMap.at(good).isLocal())
		{
			marketShare = std::max(marketShare, 1.0 / marketShareMap.size());
		}

		purchaseWeights[good] = calcPurchaseWeight(marketShare, fulfillments.at(good), culturalPrevalence.at(good));
		total += purchaseWeights.at(good);
	}

	for (const auto& good: marketShareMap | std::views::keys)
	{
		purchaseWeights.at(good) /= total == 0 ? 1.0 : total;
	}

	return purchaseWeights;
}

double V3::Market::calcPurchaseWeight(double marketShare, const GoodsFulfillment& fulfillment, const double culturalPrevalence)
{
	marketShare = std::min(marketShare, fulfillment.maxShare);
	marketShare = std::max(marketShare, fulfillment.minShare);

	// Obsessions set minimum weight at 1.
	// Note(Gawquon): Because we already averaged out the effects of taboos/obsessions,
	// this minimum weight will be slightly inaccurate in populations
	// with a high prevalence of both the taboo and the obsession for a given good.

	// Ex: A France could have 50% French pops(obsession Wine) and a 50% Muslim pops(taboo Wine).
	// We'd underestimate the Wine demand from the increase in minimum weights,
	// even though the net neutral multiplier would be accurate.

	double weight = fulfillment.weight;
	const double culturalFactor = calcCulturalFactor(culturalPrevalence);
	if (culturalFactor > 1)
	{
		const double fraction = culturalFactor - 1;
		weight = std::max(weight, fraction + (1 - fraction) * weight); // (x * 1) + (1 - x)y
	}

	return marketShare * weight * culturalFactor;
}

double V3::Market::calcCulturalNeedFactor(const std::vector<std::string>& goods, const std::map<std::string, double>& culturalPrevalence)
{
	// Taboos tap out at x0.5, Obsessions at x2.0. But we're converting from prevalence(-1 <-> 1) into the +-25% bonus/malus to the base need.
	double culturalNeedFactor = 0;
	for (const std::string& good: goods)
	{
		culturalNeedFactor += culturalPrevalence.at(good) / 4;
	}
	culturalNeedFactor = std::max(culturalNeedFactor, -0.25);
	culturalNeedFactor = std::min(culturalNeedFactor, 0.25);

	return culturalNeedFactor + 1;
}

double V3::Market::calcAddedWorkingPopFraction(const std::set<std::string>& laws, const std::map<std::string, Law>& lawsMap)
{
	return std::accumulate(laws.begin(), laws.end(), 0.0, [lawsMap](double sum, const std::string& law) {
		return sum + lawsMap.at(law).workingAdultRatioAdd;
	});
}

bool V3::Market::validateGood(const std::string& good) const
{
	if (!sellOrders.contains(good))
	{
		if (!goodsErrors.contains(good))
		{
			Log(LogLevel::Warning) << "Good: " << good << " not recognized in market. Converter will act like it doesn't exist.";
			goodsErrors.emplace(good);
		}
		return false;
	}
	return true;
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

	const auto& culturalPrevalence = estimateCulturalPrevalence(cultureData, cultures, religions);
	const auto& goodsMap = demand.getGoodsMap();

	for (const auto& [job, jobFraction]: jobData)
	{
		if (!popTypeMap.contains(job))
		{
			if (!popTypeErrors.contains(job))
			{
				Log(LogLevel::Warning) << "PopType: " << job << " has no recognized game data.";
				popTypeErrors.emplace(job);
			}
			continue;
		}
		const auto& popType = popTypeMap.at(job);

		const double popFactor = calcPopFactor(popSize * jobFraction, popType, defines, laws, lawsMap);
		const int wealth = estimateWealth(popType.getStartingQOL());

		if (!demand.getWealthConsumptionMap().contains(wealth))
		{
			if (!wealthErrors.contains(wealth))
			{
				Log(LogLevel::Warning) << "Wealth Lvl: " << wealth << " has no definition.";
				wealthErrors.emplace(wealth);
			}
			continue;
		}
		const auto& buyPackage = demand.getWealthConsumptionMap().at(wealth);

		for (const auto& [need, magnitude]: buyPackage.getPopneeds())
		{
			if (!demand.getPopneedsMap().contains(need))
			{
				if (!popneedsErrors.contains(need))
				{
					Log(LogLevel::Warning) << "Popneed: " << need << " has no definition.";
					popneedsErrors.emplace(need);
				}
				continue;
			}

			const auto& popneed = demand.getPopneedsMap().at(need);
			const auto& packageGoods = enumerateGoods(popneed.getGoodsFulfillment(), goodsMap);

			const auto& marketShareMap = getMarketShare(packageGoods);
			const auto& purchaseWeights = calcPurchaseWeights(marketShareMap, popneed.getGoodsFulfillment(), culturalPrevalence, goodsMap);
			const double culturalNeedFactor = calcCulturalNeedFactor(packageGoods, culturalPrevalence);

			// Calc Pop demand
			for (const auto& good: packageGoods)
			{
				const double basePrice = goodsMap.at(good).getBasePrice();
				const double popDemand = magnitude * culturalNeedFactor / basePrice * purchaseWeights.at(good) * popFactor;
				buyForPop(good, popDemand);
			}
		}
	}
}

void V3::Market::clearMarket()
{
	for (auto& value: sellOrders | std::views::values)
	{
		value = 0;
	}
	for (auto& value: buyOrdersPops | std::views::values)
	{
		value = 0;
	}
	for (auto& value: buyOrdersBuildings | std::views::values)
	{
		value = 0;
	}
}

// Debugging function
std::stringstream V3::Market::marketAsTable() const
{
	std::stringstream out;
	int goodLength = 0;
	int amountLength = 0;
	const auto& balance = getMarketBalance();
	for (const auto& [good, amount]: balance)
	{
		goodLength = std::max(goodLength, static_cast<int>(good.length()));
		amountLength = std::max(amountLength, static_cast<int>(std::to_string(amount).length()));
	}

	out << std::setprecision(3);
	out << std::endl;
	out << std::left << std::setw(goodLength + 2) << "Good" << std::setw(amountLength + 2) << "Percent" << std::endl;
	out << std::setfill('-') << std::setw(goodLength + 2) << "" << std::setw(amountLength + 2) << "" << std::endl;
	out << std::setfill(' ');

	std::vector<std::pair<std::string, double>> balanceVector = {balance.begin(), balance.end()};

	std::ranges::sort(balanceVector, [=](const std::pair<std::string, double>& lhs, const std::pair<std::string, double>& rhs) {
		return lhs.second < rhs.second;
	});

	for (const auto& pair: balanceVector)
	{
		if (std::abs(pair.second) < 1)
			continue;

		const double pricePercent = -0.75 * std::max(-1.0, std::min(1.0, pair.second / 100));
		out << std::left << std::setw(goodLength + 2) << pair.first << std::setw(amountLength + 2) << pricePercent * 100 << "%" << std::endl;
	}
	return out;
}
