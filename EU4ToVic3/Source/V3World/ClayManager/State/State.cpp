#include "State.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "Province.h"
#include "SubState.h"
#include <cmath>
#include <numeric>
#include <ranges>

void V3::State::loadState(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();

	distributeLandshares();
	distributeResources();
}

void V3::State::registerKeys()
{
	registerKeyword("provinces", [this](std::istream& theStream) {
		for (const auto& provinceName: commonItems::getStrings(theStream))
		{
			auto theProvinceName = provinceName;
			std::transform(theProvinceName.begin(), theProvinceName.end(), theProvinceName.begin(), ::toupper);
			if (theProvinceName.starts_with("X") && theProvinceName.size() == 7)
				theProvinceName = "x" + theProvinceName.substr(1, theProvinceName.length() - 1); // from "x12345a" to x12345A
			else
				Log(LogLevel::Warning) << "Loading province " << theProvinceName << " in unknown format!";
			const auto province = std::make_shared<Province>();
			province->setName(theProvinceName);
			provinces.emplace(theProvinceName, province);
		}
	});
	registerKeyword("prime_land", [this](std::istream& theStream) {
		for (const auto& provinceName: commonItems::getStrings(theStream))
		{
			auto theProvinceName = provinceName;
			std::transform(theProvinceName.begin(), theProvinceName.end(), theProvinceName.begin(), ::toupper);
			if (theProvinceName.starts_with("X") && theProvinceName.size() == 7)
				theProvinceName = "x" + theProvinceName.substr(1, theProvinceName.length() - 1);
			else
				Log(LogLevel::Warning) << "Encountered prime province " << theProvinceName << " in unknown format!";
			if (provinces.contains(theProvinceName))
				provinces.at(theProvinceName)->setPrime();
			else
				Log(LogLevel::Warning) << "Prime province " << theProvinceName << " isn't defined in the state! Ignoring.";
		}
	});
	registerKeyword("impassable", [this](std::istream& theStream) {
		for (const auto& provinceName: commonItems::getStrings(theStream))
		{
			auto theProvinceName = provinceName;
			std::transform(theProvinceName.begin(), theProvinceName.end(), theProvinceName.begin(), ::toupper);
			if (theProvinceName.starts_with("X") && theProvinceName.size() == 7)
				theProvinceName = "x" + theProvinceName.substr(1, theProvinceName.length() - 1);
			else
				Log(LogLevel::Warning) << "Encountered impassable province " << theProvinceName << " in unknown format!";
			if (provinces.contains(theProvinceName))
				provinces.at(theProvinceName)->setImpassable();
			else
				Log(LogLevel::Warning) << "Impassable province " << theProvinceName << " isn't defined in the state! Ignoring.";
		}
	});
	registerKeyword("traits", [this](std::istream& theStream) {
		traits = commonItems::getStrings(theStream);
	});
	registerKeyword("arable_land", [this](std::istream& theStream) {
		cappedResources["bg_agriculture"] = commonItems::getInt(theStream);
	});
	registerKeyword("arable_resources", [this](std::istream& theStream) {
		arableResources = commonItems::getStrings(theStream);
	});
	registerKeyword("capped_resources", [this](std::istream& theStream) {
		for (const auto& [resource, amount]: commonItems::assignments(theStream).getAssignments())
		{
			try
			{
				cappedResources[resource] = std::stoi(amount);
			}
			catch (const std::exception& e)
			{
				Log(LogLevel::Error) << "Failed reading amount of " << resource << " in the state: " << e.what();
			}
		}
	});
	registerKeyword("id", [this](std::istream& theStream) {
		ID = commonItems::getInt(theStream);
	});
	registerKeyword("naval_exit_id", [this](std::istream& theStream) {
		navalExitID = commonItems::getInt(theStream);
		coastal = true;
	});
	registerKeyword("port", [this](std::istream& theStream) {
		auto theProvinceName = commonItems::getString(theStream);
		std::transform(theProvinceName.begin(), theProvinceName.end(), theProvinceName.begin(), ::toupper);
		if (theProvinceName.starts_with("X") && theProvinceName.size() == 7)
			theProvinceName = "x" + theProvinceName.substr(1, theProvinceName.length() - 1);
		else
			Log(LogLevel::Warning) << "Encountered port " << theProvinceName << " in unknown format!";
		if (provinces.contains(theProvinceName))
			provinces.at(theProvinceName)->setPort();
		else
			Log(LogLevel::Warning) << "Port province " << theProvinceName << " isn't defined in the state! (Vanilla?) Map error. Ignoring port.";
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void V3::State::distributeLandshares() const
{
	const auto [stateTotal, statePrimes, stateImpassables] = countProvinceTypes(provinces);
	const double weightedStatewideProvinces = getWeightedProvinceTotals(stateTotal, statePrimes, stateImpassables);

	for (const auto& subState: substates)
	{
		const auto [subStateTotal, subStatePrimes, subStateImpassables] = countProvinceTypes(subState->getProvinces());
		const double weightedSubStateProvinces = getWeightedProvinceTotals(subStateTotal, subStatePrimes, subStateImpassables);

		double subStateLandshare = weightedSubStateProvinces / weightedStatewideProvinces;
		if (subStateLandshare < 0.05) // In defines as SPLIT_STATE_MIN_LAND_SHARE
		{
			subStateLandshare = 0.05;
		}
		subState->setLandshare(subStateLandshare);
	}
}

void V3::State::distributeResources()
{
	for (const auto& subState: substates)
	{
		for (const auto& [resource, amount]: cappedResources)
		{
			subState->setResource(resource, static_cast<int>(floor(subState->getLandshare() * amount)));
		}
	}
}

int V3::State::getWeightedProvinceTotals(const int total, const int primes, const int impassables)
{
	// prime coefficient is the define SPLIT_STATE_PRIME_LAND_WEIGHT - 1
	return total + (5 - 1) * primes - impassables;
}

std::tuple<int, int, int> V3::State::countProvinceTypes(ProvinceMap provinces)
{
	int primes = 0;
	int impassables = 0;

	for (const auto& province: std::views::values(provinces))
	{
		if (province->isPrime())
		{
			++primes;
		}
		if (province->isImpassable())
		{
			++impassables;
		}
	}

	return std::make_tuple(static_cast<int>(provinces.size()), primes, impassables);
}

std::shared_ptr<V3::Province> V3::State::getProvince(const std::string& provinceName) const
{
	if (provinces.contains(provinceName))
		return provinces.at(provinceName);
	return nullptr;
}

bool V3::State::isSea() const
{
	// We allow for a sea to be mismapped along regular provinces as vanilla has a few seas so mismapped (CHAD! KORDOFAN! Literally everywhere!).

	return std::all_of(provinces.begin(), provinces.end(), [](const std::pair<std::string, std::shared_ptr<Province>>& province) {
		return province.second->isSea();
	});
}

bool V3::State::isLake() const
{
	// We allow for a lakes to be mismapped along regular provinces as vanilla has a few lakes so mismapped.

	return std::all_of(provinces.begin(), provinces.end(), [](const std::pair<std::string, std::shared_ptr<Province>>& province) {
		return province.second->isLake();
	});
}

V3::ProvinceMap V3::State::getUnassignedProvinces() const
{
	ProvinceMap unassignedProvinces;
	std::set<std::string> seenProvinceIDs;

	for (const auto& subState: substates)
		for (const auto& provinceID: subState->getProvinces() | std::views::keys)
			seenProvinceIDs.emplace(provinceID);

	for (const auto& [provinceID, province]: provinces)
		if (!seenProvinceIDs.contains(provinceID))
			unassignedProvinces.emplace(provinceID, province);

	return unassignedProvinces;
}

bool V3::State::hasUnassignedProvinces() const
{
	// it's faster to count than to filter.
	const auto assignedProvinces = std::accumulate(substates.begin(), substates.end(), 0, [](int sum, const auto& subState) {
		return sum + static_cast<int>(subState->getProvinces().size());
	});
	if (provinces.size() - assignedProvinces == 0)
		return false;
	return true;
}

int V3::State::getStatePopCount() const
{
	return std::accumulate(substates.begin(), substates.end(), 0, [](int sum, const auto& subState) {
		return sum + subState->getSubStatePops().getPopCount();
	});
}

int V3::State::getStateSlavePopCount() const
{
	return std::accumulate(substates.begin(), substates.end(), 0, [](int sum, const auto& subState) {
		return sum + subState->getSubStatePops().getSlavePopCount();
	});
}

double V3::State::getTotalSubStateWeight() const
{
	return std::accumulate(substates.begin(), substates.end(), 0.0, [](double sum, const auto& subState) {
		if (subState->getWeight())
			return sum + *subState->getWeight();
		else
			return sum;
	});
}

std::set<std::string> V3::State::getClaims() const
{
	std::set<std::string> claims;
	for (const auto& subState: substates)
		claims.insert(subState->getClaims().begin(), subState->getClaims().end());
	return claims;
}

double V3::State::getInvestmentFactor() const
{
	double toReturn = 0;
	for (const auto& subState: substates)
		for (const auto& [data, weight]: subState->getSourceProvinceData())
			toReturn += data.investmentFactor * weight;
	toReturn /= static_cast<double>(substates.size());
	return toReturn;
}

void V3::State::distributeNonTreatyPortPops(int incomingPops)
{
	// We're taking pops from a treaty port in this state and distributing the number to other substates.

	for (const auto& subState: substates)
	{
		if (subState->isTreatyPort())
			continue;

		const auto& landShare = subState->getLandshare();
		const auto& originalSubStatePopCount = subState->getSubStatePops().getPopCount();
		const double newPopCount = originalSubStatePopCount + static_cast<int>(std::round(landShare * incomingPops));
		const double modifyFactor = newPopCount / originalSubStatePopCount;

		auto newPops = subState->getSubStatePops();
		newPops.multiplyPops(modifyFactor);

		subState->setSubStatePops(newPops);
	}
}
