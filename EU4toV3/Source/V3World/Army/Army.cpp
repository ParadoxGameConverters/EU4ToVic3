#include "Army.h"
#include "CommonFunctions.h"
#include "Log.h"
#include <algorithm>
#include <queue>
#include <random>

V2::Army::Army(const EU4::EU4Army& eu4Army,
	 std::string _tag,
	 const bool civilized,
	 const mappers::RegimentCostsMapper& regimentCostsMapper,
	 std::map<int, std::shared_ptr<Province>> allProvinces,
	 const mappers::ProvinceMapper& provinceMapper,
	 const mappers::PortProvinces& portProvincesMapper,
	 std::map<REGIMENTTYPE, int>& unitNameCount,
	 const std::string& localAdjective):
	 name(eu4Army.getName()),
	 tag(std::move(_tag))
{
	// See what we're dealing with
	for (const auto& eu4Regiment: eu4Army.getRegiments())
	{
		// Count the regiments by color.
		regimentCounts[eu4Regiment.getCategory()]++;
		// Store potential homes
		auto chosenType = pickCategory(eu4Regiment.getCategory(), civilized);
		if (eu4Regiment.getHome() > 0)
			eu4homeProvinces[chosenType].push_back(eu4Regiment.getHome());
	}
	isNavy = eu4Army.getArmyFloats();

	// Create a build order and store remainders aside.
	for (const auto& eu4RegimentItr: regimentCounts)
	{
		const auto typeStrength = eu4Army.getTotalTypeStrength(eu4RegimentItr.first);
		if (!typeStrength)
			continue; // no regiments of this type

		const auto& regimentCost = regimentCostsMapper.getCostForRegimentType(eu4RegimentItr.first);
		if (!regimentCost)
			continue; // Well, shit. Mapping error?
		auto chosenType = pickCategory(eu4RegimentItr.first, civilized);

		const auto regimentCount = static_cast<double>(typeStrength) / *regimentCost;
		buildOrder[chosenType] = static_cast<int>(std::floor(regimentCount));
		armyRemainders[chosenType] = regimentCount - buildOrder[chosenType];
	}

	// Let's build some regiments.

	for (auto buildItem: buildOrder)
	{
		for (auto regimentCounter = 0; regimentCounter < buildItem.second; ++regimentCounter)
		{
			if (addRegimentToArmy(buildItem.first, allProvinces, provinceMapper, portProvincesMapper, unitNameCount, localAdjective) !=
				 AddRegimentToArmyResult::success)
			{
				// couldn't add, dissolve into pool
				armyRemainders[buildItem.first] += 1;
			}
			else
			{
				++buildSuceeded[buildItem.first];
			}
		}
	}

	// Place the army somewhere.

	auto locationCandidates = provinceMapper.getVic2ProvinceNumbers(eu4Army.getLocation());
	if (locationCandidates.empty())
	{
		Log(LogLevel::Warning) << "no mapping for province : " << eu4Army.getLocation();
		// Mapping issues, great. Dissolve the army.
		for (auto buildItem: buildSuceeded)
		{
			armyRemainders[buildItem.first] += buildItem.second;
		}
		created = false;
		return;
	}
	if (locationCandidates.size() == 1 && *locationCandidates.begin() == 0)
	{
		Log(LogLevel::Warning) << "zero mapping for province : " << eu4Army.getLocation();
		// Good news - we have mapped the location province! Bad news, it's actually unmapped. How do we even get here?
		// Dissolve the army.
		for (auto buildItem: buildSuceeded)
		{
			armyRemainders[buildItem.first] += buildItem.second;
		}
		created = false;
		return;
	}
	auto usePort = false;
	// guarantee that navies are assigned to sea provinces, or land provinces with naval bases
	if (isNavy)
	{
		const auto& provinceItr = allProvinces.find(*locationCandidates.begin());
		if (provinceItr != allProvinces.end())
			usePort = true; // It's in land provinces, so docked.
		if (usePort)
		{
			locationCandidates = getPortProvinces(locationCandidates, allProvinces, portProvincesMapper);
			if (locationCandidates.empty())
			{
				// We have a navy and no port candidates. Yay. Better get rid of it.
				for (auto buildItem: buildSuceeded)
				{
					armyRemainders[buildItem.first] += buildItem.second;
				}
				created = false;
				return;
			}
		}
	}

	const auto selectedLocation = pickRandomProvinceID(locationCandidates);
	if (isNavy && usePort && !portProvincesMapper.isProvinceIDAllowed(selectedLocation))
	{
		LOG(LogLevel::Warning) << "Assigning navy to non-allowed port province " << selectedLocation
									  << " - if the save crashes, try deny-listing this province";
	}
	location = selectedLocation;
}

V2::REGIMENTTYPE V2::Army::pickCategory(const std::string& incCategory, const bool civilized)
{
	REGIMENTTYPE chosenType;
	if (incCategory == "infantry")
	{
		if (civilized)
			chosenType = REGIMENTTYPE::infantry;
		else
			chosenType = REGIMENTTYPE::irregular;
	}
	else if (incCategory == "cavalry")
	{
		chosenType = REGIMENTTYPE::cavalry;
	}
	else if (incCategory == "artillery")
	{
		chosenType = REGIMENTTYPE::artillery;
	}
	else if (incCategory == "heavy_ship")
	{
		chosenType = REGIMENTTYPE::manowar;
	}
	else if (incCategory == "light_ship" || incCategory == "galley")
	{
		chosenType = REGIMENTTYPE::frigate;
	}
	else if (incCategory == "transport")
	{
		chosenType = REGIMENTTYPE::clipper_transport;
	}
	else
	{
		Log(LogLevel::Warning) << "Unknown regiment category: " + incCategory + " defaulting to infantry. Hopefully this was not a ship!";
		chosenType = REGIMENTTYPE::infantry;
	}
	return chosenType;
}

// return values: 0 = success, -1 = retry from pool, -2 = do not retry
V2::AddRegimentToArmyResult V2::Army::addRegimentToArmy(const REGIMENTTYPE chosenType,
	 const std::map<int, std::shared_ptr<Province>>& allProvinces,
	 const mappers::ProvinceMapper& provinceMapper,
	 const mappers::PortProvinces& portProvincesMapper,
	 std::map<REGIMENTTYPE, int>& unitNameCount,
	 const std::string& localAdjective)
{
	// Make a regiment.
	Regiment regiment(chosenType);

	// Every regiment needs a home to draw soldiers from or to berth. Pick a home at random.
	auto eu4Home = getProbabilisticHomeProvince(chosenType);
	// We have an issue right there. Lack of eu4homes means a very broken save game. Abort.
	if (!eu4Home)
		return AddRegimentToArmyResult::fail;

	// Map the home to V2 province
	auto homeCandidates = provinceMapper.getVic2ProvinceNumbers(*eu4Home);
	if (homeCandidates.empty())
	{
		// This was a province that doesn't map to anything. Let's pretend that's fine and there's
		// no problem with province mappings file.
		blockHomeProvince(*eu4Home);
		return AddRegimentToArmyResult::retry;
	}

	// Old-style V2 province pointer. TO DO: Replace this with smart pointers.
	std::shared_ptr<Province> homeProvince = nullptr;

	if (isNavy)
	{
		// Navies should only get homes in port provinces
		homeCandidates = getPortProvinces(homeCandidates, allProvinces, portProvincesMapper);
		if (!homeCandidates.empty())
			homeProvince = pickRandomPortProvince(homeCandidates, allProvinces);
		// else: So far nothing. No berth.
	}
	else
	{
		// Armies should get a home in the candidate most capable of supporting them
		std::vector<std::shared_ptr<Province>> sortedHomeCandidates;
		for (auto candidate: homeCandidates)
		{
			auto provinceItr = allProvinces.find(candidate);
			if (provinceItr != allProvinces.end())
				sortedHomeCandidates.push_back(provinceItr->second);
		}
		sort(sortedHomeCandidates.begin(), sortedHomeCandidates.end(), provinceRegimentCapacityPredicate);
		if (!sortedHomeCandidates.empty())
		{
			for (const auto& sortedCandidate: sortedHomeCandidates)
			{
				homeProvince = sortedCandidate;
				// Check owner!
				if (homeProvince->getOwner() != tag)
					homeProvince = nullptr;
				if (homeProvince)
					break;
			}
		}
		if (!homeProvince)
		{
			// Well now. Either all candidates belong to someone else, or we have a mapping issue.
			// Or candidates belong to someone else because of mapping. Time for something drastic.
			homeProvince = getProvinceForExpeditionaryArmy(allProvinces, tag);
		}
		if (!homeProvince)
		{
			// Seriously now, not a single province with any soldiers left? Then what are we doing?
			return AddRegimentToArmyResult::fail;
		}
		///////// We have a home! Champagne and biscuits!
		// Armies need to be associated with pops
		auto soldierPop = homeProvince->getSoldierPopForArmy();
		if (!soldierPop)
		{
			// Try turning it into an "expeditionary" army - ie. assign home to any reasonable owned province.
			std::shared_ptr<Province> expSender = getProvinceForExpeditionaryArmy(allProvinces, tag);
			if (expSender)
			{
				const auto& expSoldierPop = expSender->getSoldierPopForArmy();
				if (expSoldierPop)
				{
					homeProvince = expSender;
					soldierPop = expSoldierPop;
				}
			}
		}
		if (!soldierPop)
		{
			// We failed to get any province with soldier population that can support this regiment.
			// Make it a depleted one then.
			std::shared_ptr<Province> expSender = getProvinceForExpeditionaryArmy(allProvinces, tag);
			if (expSender)
			{
				const auto& expSoldierPop = expSender->getSoldierPopForArmy(true);
				if (expSoldierPop)
				{
					homeProvince = expSender;
					soldierPop = expSoldierPop;
				}
			}
		}
		if (!soldierPop)
		{
			// We failed repeatedly to get a province for these pops, even when forcing. Time to fail.
			return AddRegimentToArmyResult::fail;
		}
		regiment.setHome(homeProvince->getID());
	}
	// Everyone except for ships at sea now has a home. Name the regiment.
	if (homeProvince)
	{
		regiment.setName(homeProvince->getRegimentName(chosenType));
	}
	else
	{
		// Assign a national name ("1st Bavarian Frigate")
		regiment.setName(getRegimentName(chosenType, unitNameCount, localAdjective));
	}
	regiments.push_back(regiment);
	return AddRegimentToArmyResult::success;
}

std::optional<int> V2::Army::getProbabilisticHomeProvince(const REGIMENTTYPE chosenType) const
{
	if (eu4homeProvinces.find(chosenType) == eu4homeProvinces.end())
		return std::nullopt;

	const auto& candidates = eu4homeProvinces.at(chosenType);
	if (candidates.empty())
		return std::nullopt;

	std::set<int> randomProvince;
	std::sample(candidates.begin(), candidates.end(), std::inserter(randomProvince, randomProvince.begin()), 1, std::mt19937{std::random_device{}()});
	return *randomProvince.begin();
}

std::shared_ptr<V2::Province> V2::Army::pickRandomPortProvince(const std::set<int>& homeCandidates,
	 const std::map<int, std::shared_ptr<Province>>& allProvinces)
{
	std::set<int> randomProvince;
	std::sample(homeCandidates.begin(), homeCandidates.end(), std::inserter(randomProvince, randomProvince.begin()), 1, std::mt19937{std::random_device{}()});

	const auto& provinceItr = allProvinces.find(*randomProvince.begin());
	if (provinceItr != allProvinces.end())
		return provinceItr->second;
	return nullptr;
}

int V2::Army::pickRandomProvinceID(std::set<int> homeCandidates)
{
	std::set<int> randomProvince;
	std::sample(homeCandidates.begin(), homeCandidates.end(), std::inserter(randomProvince, randomProvince.begin()), 1, std::mt19937{std::random_device{}()});
	if (randomProvince.empty())
		return 0;
	return *randomProvince.begin();
}

void V2::Army::blockHomeProvince(const int blocked)
{
	for (const auto& regType: RegimentTypeToName)
	{
		auto& homes = eu4homeProvinces[regType.first];
		homes.erase(std::remove(homes.begin(), homes.end(), blocked), homes.end());
	}
}

std::set<int> V2::Army::getPortProvinces(const std::set<int>& locationCandidates,
	 std::map<int, std::shared_ptr<Province>> allProvinces,
	 const mappers::PortProvinces& portProvincesMapper)
{
	std::vector<int> unblockedCandidates;
	for (auto candidate: locationCandidates)
	{
		if (!portProvincesMapper.isProvinceIDDenied(candidate))
			unblockedCandidates.push_back(candidate);
	}

	std::set<int> coastalProvinces;
	for (auto& candidate: unblockedCandidates)
	{
		auto province = allProvinces.find(candidate);
		if (province != allProvinces.end())
		{
			if (province->second->isCoastal())
			{
				coastalProvinces.insert(candidate);
			}
		}
	}
	return coastalProvinces;
}

bool V2::Army::provinceRegimentCapacityPredicate(const std::shared_ptr<Province> prov1, const std::shared_ptr<Province> prov2)
{
	return prov1->getAvailableSoldierCapacity() > prov2->getAvailableSoldierCapacity();
}

std::shared_ptr<V2::Province> V2::Army::getProvinceForExpeditionaryArmy(const std::map<int, std::shared_ptr<Province>>& allProvinces, const std::string& tag)
{
	std::vector<std::shared_ptr<V2::Province>> candidates;
	for (auto& provinceItr: allProvinces)
	{
		if (provinceItr.second->getOwner() == tag && !provinceItr.second->isColony() && !provinceItr.second->getPops("soldiers").empty())
		{
			candidates.push_back(provinceItr.second);
		}
	}
	if (!candidates.empty())
	{
		sort(candidates.begin(), candidates.end(), provinceRegimentCapacityPredicate);
		return candidates[0];
	}
	return nullptr;
}

std::string V2::Army::getRegimentName(REGIMENTTYPE chosenType, std::map<REGIMENTTYPE, int>& unitNameCount, const std::string& localAdjective)
{
	std::stringstream str;
	str << ++unitNameCount[chosenType] << cardinalToOrdinal(unitNameCount[chosenType]); // 1st, 2nd, etc
	if (localAdjective.empty())
	{
		str << " ";
	}
	else
	{
		str << " " << localAdjective << " ";
	}
	switch (chosenType)
	{
		case REGIMENTTYPE::irregular:
			str << "Irregulars";
			break;
		case REGIMENTTYPE::infantry:
			str << "Infantry";
			break;
		case REGIMENTTYPE::cavalry:
			str << "Cavalry";
			break;
		case REGIMENTTYPE::artillery:
			str << "Artillery";
			break;
		case REGIMENTTYPE::manowar:
			str << "Man'o'war";
			break;
		case REGIMENTTYPE::frigate:
			str << "Frigate";
			break;
		case REGIMENTTYPE::clipper_transport:
			str << "Clipper Transport";
			break;
	}
	return str.str();
}

bool V2::Army::hasRegimentsOfType(const REGIMENTTYPE chosenType) const
{
	for (const auto& regiment: regiments)
		if (regiment.getType() == chosenType)
			return true;
	return false;
}

double V2::Army::getRegimentRemainder(const REGIMENTTYPE chosenType) const
{
	const auto& remainder = armyRemainders.find(chosenType);
	if (remainder != armyRemainders.end())
		return remainder->second;
	return 0;
}
