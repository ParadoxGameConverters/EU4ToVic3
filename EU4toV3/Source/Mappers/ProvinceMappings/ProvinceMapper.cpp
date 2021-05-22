#include "ProvinceMapper.h"
#include "Configuration.h"
#include "GameVersion.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "ProvinceMappingsVersion.h"
#include <filesystem>
#include <fstream>
#include <stdexcept>
namespace fs = std::filesystem;
#include "CommonRegexes.h"

mappers::ProvinceMapper::ProvinceMapper(): colonialRegionsMapper(std::make_unique<EU4::ColonialRegions>())
{
	LOG(LogLevel::Info) << "Parsing province mappings";
	registerKeys();
	if (theConfiguration.isHpmEnabled())
	{
		parseFile("configurables/HPM/province_mappings.txt");
	}
	else
	{
		parseFile("configurables/province_mappings.txt");
	}
	clearRegisteredKeywords();

	const auto& mappings = getBestMappingsVersion(theConfiguration.getEU4Version());
	createMappings(mappings);
}

mappers::ProvinceMapper::ProvinceMapper(std::istream& mainStream, std::istream& colonialStream, const Configuration& testConfiguration):
	 colonialRegionsMapper(std::make_unique<EU4::ColonialRegions>(colonialStream))
{
	registerKeys();
	parseStream(mainStream);
	clearRegisteredKeywords();

	const auto& mappings = getBestMappingsVersion(testConfiguration.getEU4Version());
	createMappings(mappings);
}

void mappers::ProvinceMapper::registerKeys()
{
	registerRegex(R"([\d.]+)", [this](const std::string& versionString, std::istream& theStream) {
		ProvinceMappingsVersion mappingVersion(versionString, theStream);
		mappingVersions.insert(std::make_pair(mappingVersion.getVersion(), mappingVersion));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

std::optional<std::string> mappers::ProvinceMapper::getColonialRegionForProvince(int province) const
{
	return colonialRegionsMapper->getColonialRegionForProvince(province);
}

mappers::ProvinceMappingsVersion mappers::ProvinceMapper::getBestMappingsVersion(const GameVersion& usedEU4Version) const
{
	for (auto mappingVersion = mappingVersions.rbegin(); mappingVersion != mappingVersions.rend(); ++mappingVersion)
	{
		if (usedEU4Version >= mappingVersion->first)
		{
			LOG(LogLevel::Info) << "\t-> Using version " << mappingVersion->first << " mappings";
			return mappingVersion->second;
		}
	}
	throw std::range_error("Could not find matching province mappings for EU4 version");
}

void mappers::ProvinceMapper::createMappings(const ProvinceMappingsVersion& provinceMappingsVersion)
{
	for (const auto& mapping: provinceMappingsVersion.getMappings())
	{
		// ignore deliberate errors where we leave mappings without keys (asian wasteland comes to mind):
		if (mapping.getVic2Provinces().empty())
			continue;
		if (mapping.getEU4Provinces().empty())
			continue;

		for (const auto& vic2Number: mapping.getVic2Provinces())
		{
			if (vic2Number)
				vic2ToEU4ProvinceMap.insert(std::make_pair(vic2Number, mapping.getEU4Provinces()));
		}
		for (const auto& eu4Number: mapping.getEU4Provinces())
		{
			if (eu4Number)
				eu4ToVic2ProvinceMap.insert(std::make_pair(eu4Number, mapping.getVic2Provinces()));
		}
	}
}

std::set<int> mappers::ProvinceMapper::getVic2ProvinceNumbers(const int eu4ProvinceNumber) const
{
	const auto& mapping = eu4ToVic2ProvinceMap.find(eu4ProvinceNumber);
	if (mapping != eu4ToVic2ProvinceMap.end())
		return mapping->second;
	else
		return std::set<int>();
}

std::set<int> mappers::ProvinceMapper::getEU4ProvinceNumbers(int vic2ProvinceNumber) const
{
	const auto& mapping = vic2ToEU4ProvinceMap.find(vic2ProvinceNumber);
	if (mapping != vic2ToEU4ProvinceMap.end())
		return mapping->second;
	else
		return std::set<int>();
}
