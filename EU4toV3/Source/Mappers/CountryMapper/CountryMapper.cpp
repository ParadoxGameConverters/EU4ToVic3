#include "CountryMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <iomanip>

void mappers::CountryMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing Country Mapping Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << countryMappingRules.size() << " Rules loaded.";
}

void mappers::CountryMapper::loadMappingRules(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CountryMapper::registerKeys()
{
	registerKeyword("link", [this](std::istream& theStream) {
		const CountryMapping newMapping(theStream);
		countryMappingRules.emplace_back(newMapping);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

bool mappers::CountryMapper::tagIsAlphaDigitDigit(const std::string& tag)
{
	return isalpha(tag[0]) && isdigit(tag[1]) && isdigit(tag[2]);
}

bool mappers::CountryMapper::tagIsAlphaDigitAlphaNum(const std::string& tag)
{
	return isalpha(tag[0]) && isdigit(tag[1]) && isalnum(tag[2]);
}

std::string mappers::CountryMapper::generateNewTag()
{
	std::ostringstream generatedV3TagStream;
	generatedV3TagStream << generatedV3TagPrefix << std::setfill('0') << std::setw(2) << generatedV3TagSuffix;
	const auto& v3Tag = generatedV3TagStream.str();

	++generatedV3TagSuffix;
	if (generatedV3TagSuffix > 99)
	{
		generatedV3TagSuffix = 0;
		--generatedV3TagPrefix;
	}

	return v3Tag;
}

bool mappers::CountryMapper::tagIsAlreadyAssigned(const std::string& v3Tag) const
{
	return v3TagToEU4TagMap.contains(v3Tag);
}

std::optional<std::string> mappers::CountryMapper::getV2Tag(const std::string& eu4Tag) const
{
	// This is an override for anything that looks like a rebellious title.
	const std::set<std::string> eu4RebelTags = {"REB", "PIR", "NAT"};
	if (eu4RebelTags.contains(eu4Tag))
		return "REB";

	if (eu4TagToV3TagMap.contains(eu4Tag))
		return eu4TagToV3TagMap.at(eu4Tag);

	return std::nullopt;
}

std::optional<std::string> mappers::CountryMapper::getEU4Tag(const std::string& v3Tag) const
{
	if (v3TagToEU4TagMap.contains(v3Tag))
		return v3TagToEU4TagMap.at(v3Tag);

	return std::nullopt;
}
