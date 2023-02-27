#include "ProvinceMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ProvinceMapping.h"

void mappers::ProvinceMapper::loadProvinceMappings(const std::string& fileName)
{
	Log(LogLevel::Info) << "-> Parsing province mapping rules.";
	registerKeys();
	parseFile(fileName);
	clearRegisteredKeywords();
	createMappings();
	Log(LogLevel::Info) << "<> " << mappings.size() << " rules loaded.";
}

void mappers::ProvinceMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& unused, std::istream& theStream) {
		// We support only a single, current version, so eu4-vic2 style multiple versions
		// have been cut (for now). There should only be a single, some.name={} block inside province_mappings.txt
		if (mappings.empty())
		{
			mappings = ProvinceMappingsVersion(theStream).getMappings();
		}
		else
		{
			commonItems::ignoreItem(unused, theStream);
		}
	});
}

void mappers::ProvinceMapper::createMappings()
{
	for (const auto& mapping: mappings)
	{
		// fix deliberate errors where we leave mappings without keys (wastelands and lakes comes to mind):
		if (mapping.getV3Provinces().empty())
			continue;
		if (mapping.getEU4Provinces().empty())
			continue;

		for (const auto& v3Name: mapping.getV3Provinces())
		{
			if (!v3Name.empty())
				V3ToEU4ProvinceMap.emplace(v3Name, mapping.getEU4Provinces());
		}
		for (const auto& eu4Number: mapping.getEU4Provinces())
		{
			if (eu4Number > 0)
				EU4ToVic3ProvinceMap.emplace(eu4Number, mapping.getV3Provinces());
		}
	}
}

std::vector<int> mappers::ProvinceMapper::getEU4Provinces(const std::string& v3Province) const
{
	if (V3ToEU4ProvinceMap.contains(v3Province))
		return V3ToEU4ProvinceMap.at(v3Province);

	return {};
}

std::vector<std::string> mappers::ProvinceMapper::getV3Provinces(int eu4Province) const
{
	if (EU4ToVic3ProvinceMap.contains(eu4Province))
		return EU4ToVic3ProvinceMap.at(eu4Province);

	return {};
}
