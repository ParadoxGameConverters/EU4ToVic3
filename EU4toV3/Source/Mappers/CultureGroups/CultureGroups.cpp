#include "CultureGroups.h"
#include "Configuration.h"
#include "Culture.h"
#include "CultureGroup.h"
#include "CultureMapper/CultureMapper.h"
#include "EU4World/World.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

void mappers::CultureGroups::initForEU4()
{
	LOG(LogLevel::Info) << "Parsing Cultures and Culture Groups";
	registerKeys();

	for (const auto& cultureFile: commonItems::GetAllFilesInFolder(theConfiguration.getEU4Path() + "/common/cultures/"))
		parseFile(theConfiguration.getEU4Path() + "/common/cultures/" + cultureFile);

	for (const auto& modPath: theConfiguration.getEU4Mods())
		for (const auto& cultureFile: commonItems::GetAllFilesInFolder(modPath + "/common/cultures/"))
			parseFile(modPath + "/common/cultures/" + cultureFile);

	clearRegisteredKeywords();
}

void mappers::CultureGroups::initForV2()
{
	LOG(LogLevel::Info) << "Parsing V2 Cultures and Culture Groups";
	registerKeys();
	parseFile("configurables/culture_definitions.txt");
	clearRegisteredKeywords();
}

mappers::CultureGroups::CultureGroups(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureGroups::registerKeys()
{
	registerRegex(R"([\w_]+)", [this](std::string cultureGroupName, std::istream& theStream) {
		auto newGroup = std::make_shared<CultureGroup>(cultureGroupName, theStream);

		if (const auto& hpm = newGroup->getHpm(); !hpm.empty())
		{
			if (!theConfiguration.isHpmEnabled() && hpm == "yes")
				return;
			if (theConfiguration.isHpmEnabled())
			{
				if (hpm == "no")
					return;
				else if (hpm != "yes")
					cultureGroupName = hpm;
			}
		}

		if (cultureGroupsMap.contains(cultureGroupName))
		{
			// We would normally override base definitions with incoming CK2 ones, but CK2 definitions
			// are crap and don't actually list all required cultures, so we have to merge.
			for (const auto& [cultureName, culture]: newGroup->getCultures())
				cultureGroupsMap[cultureGroupName]->mergeCulture(cultureName, culture);
		}
		else
		{
			cultureGroupsMap.insert(std::make_pair(cultureGroupName, newGroup));
		}
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::shared_ptr<mappers::CultureGroup> mappers::CultureGroups::getGroupForCulture(const std::string& cultureName) const
{
	for (const auto& cultureGroupItr: cultureGroupsMap)
		if (cultureGroupItr.second->containsCulture(cultureName))
			return cultureGroupItr.second;
	return nullptr;
}

void mappers::CultureGroups::importNeoCultures(const EU4::Regions& regions,
	 const std::shared_ptr<CultureGroups>& eu4CultureGroupsMapper,
	 const CultureMapper& cultureMapper) const
{
	for (const auto& [eu4CultureGroupName, eu4CultureGroup]: eu4CultureGroupsMapper->getCultureGroupsMap())
	{
		for (const auto& [eu4CultureName, eu4Culture]: eu4CultureGroup->getCultures())
		{
			// Hello random eu4 culture. Are you a neoculture?
			if (!eu4Culture->isNeoCulture())
				continue;

			// Let's find out what V2 culture group this one is supposed to belong to.
			const auto& origEU4CultureName = eu4Culture->getOriginalCulture();
			const auto& destV2CultureName = cultureMapper.cultureNonRegionalNonReligiousMatch(regions, origEU4CultureName, "", 0, "");
			if (!destV2CultureName)
			{
				// yeah let's not go there.
				Log(LogLevel::Warning) << "Unable to locate culture mapping for EU4 culture: " << origEU4CultureName << ". This will end in tears.";
				continue;
			}
			const auto& destV2CultureGroup = getGroupForCulture(*destV2CultureName);
			if (!destV2CultureGroup)
			{
				// let's not go there either.
				Log(LogLevel::Warning) << "Unable to locate culture group for V2 culture: " << *destV2CultureName << ". This will end in blood.";
				continue;
			}

			// Now transmogrify eu4 culture definitions into V2 culture definitions.
			auto v2Culture = std::make_shared<Culture>(*eu4Culture);
			v2Culture->transmogrify();

			// and file under appropriate group.
			destV2CultureGroup->addNeoCulture(eu4CultureName, v2Culture, eu4CultureName);
		}
	}
}
