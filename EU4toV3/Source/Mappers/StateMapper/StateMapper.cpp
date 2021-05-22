#include "StateMapper.h"
#include "../../Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"

mappers::StateMapper::StateMapper()
{
	LOG(LogLevel::Info) << "Parsing state region structure.";
	registerKeys();

	std::string filename;
	if (commonItems::DoesFileExist("./blankMod/output/map/region.txt"))
	{
		filename = "./blankMod/output/map/region.txt";
	}
	else
	{
		filename = theConfiguration.getVic2Path() + "/map/region.txt";
	}

	parseFile(filename);
	clearRegisteredKeywords();
}


mappers::StateMapper::StateMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::StateMapper::registerKeys()
{
	registerRegex("[a-zA-Z0-9_]+", [this](const std::string& unused, std::istream& theStream) 
		{
			const commonItems::intList provinceList(theStream);

			std::set<int> provinces;
			for (auto province : provinceList.getInts()) provinces.insert(province);
			for (auto province : provinces) stateProvincesMap.insert(std::make_pair(province, provinces));
		});
}

std::set<int> mappers::StateMapper::getAllProvincesInState(const int province) const
{
	const auto& mapping = stateProvincesMap.find(province);
	if (mapping != stateProvincesMap.end()) return mapping->second;
	std::set<int> empty;
	return empty;
}
