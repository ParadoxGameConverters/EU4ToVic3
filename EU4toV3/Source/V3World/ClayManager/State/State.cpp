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
			std::ranges::transform(theProvinceName.begin(), theProvinceName.end(), theProvinceName.begin(), ::toupper);
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
			std::ranges::transform(theProvinceName.begin(), theProvinceName.end(), theProvinceName.begin(), ::toupper);
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
			std::ranges::transform(theProvinceName.begin(), theProvinceName.end(), theProvinceName.begin(), ::toupper);
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
		cappedResources["arable_land"] = commonItems::getInt(theStream);
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
	registerKeyword("naval_exit_id", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		coastal = true;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void V3::State::distributeLandshares() const
{
	const auto [stateTotal, statePrimes, stateImpassables] = countProvinceTypes(provinces);
	const double weightedStatewideProvinces = getWeightedProvinceTotals(stateTotal, statePrimes, stateImpassables);

	for (const auto& substate: substates)
	{
		const auto [substateTotal, substatePrimes, substateImpassables] = countProvinceTypes(substate->getProvinces());
		const double weightedSubstateProvinces = getWeightedProvinceTotals(substateTotal, substatePrimes, substateImpassables);

		double substateLandshare = weightedSubstateProvinces / weightedStatewideProvinces;
		if (substateLandshare < 0.05) // In defines as SPLIT_STATE_MIN_LAND_SHARE
		{
			substateLandshare = 0.05;
		}
		substate->setLandshare(substateLandshare);
	}
}

void V3::State::distributeResources()
{
	for (const auto& substate: substates)
	{
		for (const auto& [resource, amount]: cappedResources)
		{
			substate->setResource(resource, static_cast<int>(floor(substate->getLandshare() * amount)));
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

	return std::ranges::all_of(provinces.begin(), provinces.end(), [](const std::pair<std::string, std::shared_ptr<Province>>& province) {
		return province.second->isSea();
	});
}

bool V3::State::isLake() const
{
	// We allow for a lakes to be mismapped along regular provinces as vanilla has a few lakes so mismapped.

	return std::ranges::all_of(provinces.begin(), provinces.end(), [](const std::pair<std::string, std::shared_ptr<Province>>& province) {
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

double V3::State::getTotalSubStateWeight() const
{
	return std::accumulate(substates.begin(), substates.end(), 0.0, [](double sum, const auto& subState) {
		if (subState->getWeight())
			return sum + *subState->getWeight();
		else
			return sum;
	});
}
