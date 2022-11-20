#include "CountryMapper.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
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

std::optional<std::string> mappers::CountryMapper::getV3Tag(const std::string& eu4Tag) const
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

std::optional<std::string> mappers::CountryMapper::getFlagCode(const std::string& v3Tag) const
{
	if (v3FlagCodes.contains(v3Tag))
		return v3FlagCodes.at(v3Tag);

	return std::nullopt;
}

std::string mappers::CountryMapper::assignV3TagToEU4Country(const std::shared_ptr<EU4::Country>& country)
{
	// Is this country already mapped?
	const auto& eu4Tag = country->getTag();
	if (eu4TagToV3TagMap.contains(eu4Tag))
		return eu4TagToV3TagMap.at(eu4Tag);

	// prep filtering details.
	const auto& eu4Name = country->getName("english");
	const auto& countryFlags = country->getFlags();
	const auto& countryReforms = country->getReforms();

	// and start poking rules.
	for (const auto& rule: countryMappingRules)
	{
		// Do we have blocking flags or reforms?
		const bool flagLock = clearLocks(rule.getFlags(), countryFlags);
		const bool reformLock = clearLocks(rule.getReforms(), countryReforms);

		// are we clear?
		if (flagLock || reformLock) // either blocks, we're failing.
			continue;

		// Only do name and tag if there's something to be done here.
		if (rule.getName() || rule.getEU4Tag())
		{
			// Do we have a blocking name or blocking tag?
			const auto nameBlock = clearBlock(rule.getName(), eu4Name);
			const auto tagBlock = clearBlock(rule.getEU4Tag(), eu4Tag);

			// are we clear?
			if ((!nameBlock || *nameBlock) && (!tagBlock || *tagBlock)) // to continue we need either one of these BOTH existing and not blocking.
				continue;
		}

		// We're in the clear. Do we HAVE a target tag?
		std::string v3Tag;
		if (rule.getV3Tag())
			v3Tag = *rule.getV3Tag();
		else
			v3Tag = generateNewTag();

		// file a relation and wrap up.
		mapToTag(eu4Tag, v3Tag, rule.getFlagCode());
		return v3Tag;
	}

	// No rules match. Generate, file and wrap it up.
	auto v3Tag = generateNewTag();
	mapToTag(eu4Tag, v3Tag, std::nullopt);
	return v3Tag;
}

bool mappers::CountryMapper::clearLocks(const std::set<std::string>& ruleLocks, const std::set<std::string>& countryKeys)
{
	bool locker = false;
	if (!ruleLocks.empty())
	{
		locker = true;
		for (const auto& lock: ruleLocks)
			if (countryKeys.contains(lock))
			{
				locker = false;
				break;
			}
	}
	return locker;
}

std::optional<bool> mappers::CountryMapper::clearBlock(const std::optional<std::string>& ruleString, const std::string& countryString)
{
	if (!ruleString)
		return std::nullopt;
	bool locker = false;
	if (ruleString)
	{
		locker = true;
		if (*ruleString == countryString)
			locker = false;
	}
	return locker;
}

void mappers::CountryMapper::mapToTag(const std::string& eu4Tag, const std::string& v3Tag, const std::optional<std::string>& flagCode)
{
	eu4TagToV3TagMap.emplace(eu4Tag, v3Tag);
	v3TagToEU4TagMap.emplace(v3Tag, eu4Tag);
	if (flagCode)
		v3FlagCodes.emplace(v3Tag, *flagCode);
}
