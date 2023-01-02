#include "FlagColorLoader.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::FlagColorLoader::loadFlagColors(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Loading EU4 custom flag colors.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << flagColorMapping.size() << " EU4 custom flag colors.";
}

void V3::FlagColorLoader::registerKeys()
{
	registerKeyword("flag_color", [this](std::istream& theStream) {
		const auto theColor = commonItems::Color::Factory{}.getColor(theStream);
		flagColorMapping.emplace_back(theColor);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<commonItems::Color> V3::FlagColorLoader::getFlagColorByIndex(int index) const
{
	if (flagColorMapping.empty() || static_cast<size_t>(index) > flagColorMapping.size() - 1)
		return std::nullopt;

	return flagColorMapping.at(index);
}
