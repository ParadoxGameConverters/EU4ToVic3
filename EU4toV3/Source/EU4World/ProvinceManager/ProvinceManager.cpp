#include "ProvinceManager.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

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

const std::shared_ptr<EU4::Province>& EU4::ProvinceManager::getProvince(const int provinceID) const
{
	if (!provinces.contains(provinceID))
		throw std::runtime_error(
			 std::string("Accessing EU4 province ") + std::to_string(provinceID) + std::string(" that does not exist (Save/EU4 version mismatch?)"));
	else
		return provinces.at(provinceID);
}

void EU4::ProvinceManager::loadParsers(const std::string& EU4Path, const Mods& mods)
{
	defaultMapParser.loadDefaultMap(EU4Path, mods);
	definitionScraper.loadDefinitions(EU4Path, mods);
}

void EU4::ProvinceManager::classifyProvinces(const RegionManager& regionManager)
{
	// We are poking at mapdata to classify provinces as land, sea, lake, rnw or wasteland.
	// Then we'll be discarding lakes, rnws and wastelands.

	std::map<int, std::shared_ptr<Province>> viableProvinces;

	for (const auto& [provinceID, province]: provinces)
	{
		if (!definitionScraper.isValidID(provinceID))
		{
			// This means save contains province(s) not present in definitions. This means we're running against old or broken EU4 installation.
			// Continuing is not advised.
			Log(LogLevel::Error) << "Province " << provinceID << " was not found in map/definition.csv.";
			Log(LogLevel::Error) << "You are converting a newer save against an old EU4 installation, or you have mods that alter the map in a broken fashion.";
			Log(LogLevel::Error) << "Upgrade installation or fix the mod map definition errors to continue.";
			throw std::runtime_error("Unable to continue. Province inconsistency detected.");
		}

		if (defaultMapParser.isLake(provinceID)) // discard lake
			continue;
		if (defaultMapParser.isRNW(provinceID)) // discard RNW
			continue;
		if (defaultMapParser.isSea(provinceID))
		{
			// keep sea and classify
			viableProvinces.emplace(provinceID, province);
			province->setSea();
			continue;
		}
		if (!regionManager.provinceIsValid(provinceID)) // regionManager considers wastelands invalid, as they aren't registered.
			continue;
		// Whatever remains is a legit land province
		viableProvinces.emplace(provinceID, province);
	}
	provinces.swap(viableProvinces);
	Log(LogLevel::Info) << "<> Provinces filtered, " << provinces.size() << " remain.";
}
