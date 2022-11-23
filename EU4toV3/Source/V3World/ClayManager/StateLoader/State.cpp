#include "State.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "StringUtils.h"
#include "V3World/ClayManager/ProvinceTypeCounter.h"
#include "V3World/ClayManager/SubState.h"
#include <ranges>


void V3::State::loadState(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::State::registerKeys()
{
	registerKeyword("provinces", [this](std::istream& theStream) {
		for (const auto& provinceName: commonItems::getStrings(theStream))
		{
			auto theProvinceName = commonItems::remQuotes(provinceName);
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
			auto theProvinceName = commonItems::remQuotes(provinceName);
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
			auto theProvinceName = commonItems::remQuotes(provinceName);
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
		std::transform(traits.begin(), traits.end(), traits.begin(), commonItems::remQuotes);
	});
	registerKeyword("arable_land", [this](std::istream& theStream) {
		cappedResources["arable_land"] = commonItems::getInt(theStream);
	});
	registerKeyword("arable_resources", [this](std::istream& theStream) {
		for (const auto& resource: commonItems::getStrings(theStream))
		{
			arableResources[commonItems::remQuotes(resource)] = true;
		}
	});
	registerKeyword("capped_resources", [this](std::istream& theStream) {
		for (const auto& resource: commonItems::getStrings(theStream))
		{
			cappedResources[resource] = commonItems::getInt(theStream);
		}
	});
	registerKeyword("naval_exit_id", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		coastal = true;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void V3::State::distributeLandshares()
{
	const auto statewideProvinceTypes = countProvinceTypes(provinces);
	double weightedStatewideProvinces = calculateWeightedProvinceTotals(*statewideProvinceTypes);

	for (const auto& substate: substates)
	{
		const auto substateCount = countProvinceTypes(substate->provinces);
		double weightedSubstateProvinces = calculateWeightedProvinceTotals(*substateCount);

		double substateLandshare = weightedSubstateProvinces / weightedStatewideProvinces;
		if (substateLandshare < 0.05) // In defines as SPLIT_STATE_MIN_LAND_SHARE
		{
			substateLandshare = 0.05;
		}
		substate->landshare = substateLandshare;
	}
}

int V3::State::calculateWeightedProvinceTotals(const ProvinceTypeCounter& theCount)
{
	// prime coeffcient is the define SPLIT_STATE_PRIME_LAND_WEIGHT - 1
	return theCount.every + (5 - 1) * theCount.prime - theCount.impassable;
}

const std::unique_ptr<V3::ProvinceTypeCounter> V3::State::countProvinceTypes(std::map<std::string, std::shared_ptr<Province>> provinces)
{
	auto typeCounter = std::unique_ptr<V3::ProvinceTypeCounter>();

	typeCounter->every = provinces.size();
	for (const auto& province: std::views::values(provinces))
	{
		if (province->isPrime())
		{
			++typeCounter->prime;
		}
		if (province->isImpassable())
		{
			++typeCounter->impassable;
		}
	}

	return std::move(typeCounter);
}

std::shared_ptr<V3::Province> V3::State::getProvince(const std::string& provinceName) const
{
	if (provinces.contains(provinceName))
		return provinces.at(provinceName);
	return nullptr;
}