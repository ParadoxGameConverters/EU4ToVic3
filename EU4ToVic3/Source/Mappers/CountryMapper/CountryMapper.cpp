#include "CountryMapper.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <iomanip>

void mappers::CountryMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing country mapping rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << countryMappingRules.size() << " rules loaded.";
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

bool mappers::CountryMapper::tagIsDynamic(const std::string& tag) const
{
	if (tag.size() < 3) // A shorter tag is certainly dynamic. Also probably illegal but that isn't our problem.
		return true;

	if (dynamicallyGeneratedTags.contains(tag))
		return true;

	// Generally a strong check for colonial tags, eg. C04. Can be used for other purposes.
	return isalpha(tag[0]) && isdigit(tag[1]) && isdigit(tag[2]);
}

bool mappers::CountryMapper::tagIsNonCanon(const std::string& tag) const
{
	if (tag.size() < 3) // A shorter tag is certainly non-canon. Also probably illegal but that isn't our problem.
		return true;

	if (dynamicallyGeneratedTags.contains(tag))
		return true;

	// A truism for non-canon countries, generally both dynamic C04, and converter-generated (by CK3toEU4), eg. Z0A, Y0B..
	return isalpha(tag[0]) && isdigit(tag[1]) && isalnum(tag[2]);
}

std::string mappers::CountryMapper::generateNewTag()
{
	std::string v3Tag;
	do
	{
		std::ostringstream generatedV3TagStream;
		generatedV3TagStream << generatedV3TagPrefix << std::setfill('0') << std::setw(2) << generatedV3TagSuffix;
		v3Tag = generatedV3TagStream.str();

		++generatedV3TagSuffix;
		if (generatedV3TagSuffix > 99)
		{
			generatedV3TagSuffix = 0;
			--generatedV3TagPrefix;
		}
	} while (tagIsAlreadyAssigned(v3Tag) || knownVanillaV3Tags.contains(v3Tag));
	dynamicallyGeneratedTags.emplace(v3Tag);
	return v3Tag;
}

bool mappers::CountryMapper::tagIsAlreadyAssigned(const std::string& v3Tag) const
{
	return v3TagToEU4TagMap.contains(v3Tag) || unmappedV3Tags.contains(v3Tag);
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
		const bool flagLock = existsLocks(rule.getFlags(), countryFlags);
		const bool reformLock = existsLocks(rule.getReforms(), countryReforms);

		// are we clear?
		if (flagLock || reformLock) // either blocks, we're failing.
			continue;

		// Only do name and tag if there's something to be done here.
		if (rule.getName() || rule.getEU4Tag())
		{
			// Do we have a blocking name or blocking tag?
			const auto nameBlock = existsBlock(rule.getName(), eu4Name);
			const auto tagBlock = existsBlock(rule.getEU4Tag(), eu4Tag);

			// are we clear?
			if ((!nameBlock || *nameBlock) && (!tagBlock || *tagBlock)) // to continue we need either one of these BOTH existing and not blocking.
				continue;
		}

		// file a relation and wrap up.
		return mapToTag(eu4Tag, rule.getV3Tag(), rule.getFlagCode());
	}

	// No rules match. Generate, file and wrap it up.
	return mapToTag(eu4Tag, std::nullopt, std::nullopt);
}

bool mappers::CountryMapper::existsLocks(const std::set<std::string>& ruleLocks, const std::set<std::string>& countryKeys)
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

std::optional<bool> mappers::CountryMapper::existsBlock(const std::optional<std::string>& ruleString, const std::string& countryString)
{
	if (!ruleString)
		return std::nullopt;
	if (*ruleString == countryString)
		return false;
	return true;
}

std::string mappers::CountryMapper::mapToTag(const std::string& eu4Tag, const std::optional<std::string>& v3Tag, const std::optional<std::string>& flagCode)
{
	std::string newTag;
	if (v3Tag)
	{
		if (tagIsAlreadyAssigned(*v3Tag)) // it it taken already?
			newTag = generateNewTag();
		else
			newTag = *v3Tag;
	}
	else
	{
		newTag = generateNewTag();
	}
	eu4TagToV3TagMap.emplace(eu4Tag, newTag);
	v3TagToEU4TagMap.emplace(newTag, eu4Tag);
	if (flagCode)
		v3FlagCodes.emplace(newTag, *flagCode);

	return newTag;
}

std::string mappers::CountryMapper::requestNewV3Tag()
{
	auto newTag = generateNewTag();
	unmappedV3Tags.emplace(newTag);
	return newTag;
}

void mappers::CountryMapper::relink(const std::string& eu4tag, const std::string& currentTag, const std::string& newTag)
{
	if (!eu4TagToV3TagMap.contains(eu4tag))
		Log(LogLevel::Error) << "Relinking eu4 tag " + eu4tag + " failed, it was never linked!";

	eu4TagToV3TagMap.at(eu4tag) = newTag;
	v3TagToEU4TagMap.erase(currentTag);
	v3TagToEU4TagMap.emplace(newTag, eu4tag);
	if (v3FlagCodes.contains(currentTag))
	{
		v3FlagCodes.emplace(newTag, v3FlagCodes.at(currentTag));
		v3FlagCodes.erase(currentTag);
	}
}
