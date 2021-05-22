#include "FlagColorMapper.h"
#include "Configuration.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::FlagColorMapper::FlagColorMapper()
{
	LOG(LogLevel::Info) << "Parsing EU4 custom flag colours.";
	registerKeys();
	parseFile(theConfiguration.getEU4Path() + "/common/custom_country_colors/00_custom_country_colors.txt");
	clearRegisteredKeywords();
}

mappers::FlagColorMapper::FlagColorMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::FlagColorMapper::registerKeys()
{
	registerKeyword("flag_color", [this](const std::string& unused, std::istream& theStream) {
		const auto theColor = commonItems::Color::Factory{}.getColor(theStream);
		flagColorMapping.emplace_back(theColor);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<commonItems::Color> mappers::FlagColorMapper::getFlagColorByIndex(int index) const
{
	if (flagColorMapping.empty() || static_cast<size_t>(index) >= flagColorMapping.size() - 1)
		return std::nullopt;
	else
		return flagColorMapping.at(index);
}
