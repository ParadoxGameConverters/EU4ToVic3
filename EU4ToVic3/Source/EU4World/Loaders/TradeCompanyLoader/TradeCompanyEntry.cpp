#include "TradeCompanyEntry.h"
#include "CommonRegexes.h"
#include "ParserHelpers.h"
#include "StringUtils.h"
#include "sstream"

EU4::TradeCompanyEntry::TradeCompanyEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::TradeCompanyEntry::registerKeys()
{
	registerKeyword("color", [this](std::istream& theStream) {
		tradeCompany.color = commonItems::Color::Factory().getColor(theStream);
	});
	registerKeyword("provinces", [this](std::istream& theStream) {
		auto provinces = commonItems::getInts(theStream);
		tradeCompany.provinces.insert(provinces.begin(), provinces.end());
	});
	registerKeyword("names", [this](std::istream& theStream) {
		const auto entry = commonItems::stringOfItem(theStream).getString();
		if (entry.find("trigger") != std::string::npos) // let's not go there - triggers need actual comprehension.
			return;
		std::stringstream theEntry(entry);
		auto names = commonItems::assignments(theEntry).getAssignments();
		if (names.contains("name"))
			tradeCompany.locNameKeys.emplace_back(commonItems::remQuotes(names.at("name")));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
