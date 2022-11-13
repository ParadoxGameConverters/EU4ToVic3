#include "State.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "StringUtils.h"

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
	registerKeyword("naval_exit_id", [this](const std::string& unused, std::istream& theStream) {
		commonItems::ignoreItem(unused, theStream);
		coastal = true;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
