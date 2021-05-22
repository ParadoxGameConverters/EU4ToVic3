#include "ColonialFlagsMapper.h"
#include "Log.h"
#include "CommonRegexes.h"

mappers::ColonialFlagsMapper::ColonialFlagsMapper()
{
	Log(LogLevel::Info) << "Loading colonial flag registry.";
	registerKeys();
	parseFile("configurables/colonial_flags.txt");
	clearRegisteredKeywords();
}

mappers::ColonialFlagsMapper::ColonialFlagsMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::ColonialFlagsMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& region, std::istream& theStream) {
		const ColonialFlagRegion newRegion(theStream, region);
		auto newRegionFlags = newRegion.getRegionalFlags();
		colonialFlags.insert(newRegionFlags.begin(), newRegionFlags.end());
	});
}

std::optional<mappers::ColonialFlag> mappers::ColonialFlagsMapper::getFlag(const std::string& name) const
{

	if (const auto& possibleFlag = colonialFlags.find(name); possibleFlag != colonialFlags.end())
		return possibleFlag->second;
	for (auto flag: colonialFlags)
		if (name.find(flag.first) != std::string::npos)
			return flag.second;
	return std::nullopt;
}

std::vector<std::string> mappers::ColonialFlagsMapper::getCommonNames() const
{
	std::vector<std::string> names;
	for (const auto& flag: colonialFlags)
		if (!flag.second.isUnique())
			names.push_back(flag.first);
	return names;
}

void mappers::ColonialFlagsMapper::removeFlag(const std::string& name)
{
	colonialFlags.erase(name);
}
