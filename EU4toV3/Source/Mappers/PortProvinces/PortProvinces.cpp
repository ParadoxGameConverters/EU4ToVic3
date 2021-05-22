#include "PortProvinces.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

mappers::PortProvinces::PortProvinces()
{
	Log(LogLevel::Info) << "Parsing black and white ports.";
	registerKeys();
	parseFile("configurables/port_provinces.txt");
	clearRegisteredKeywords();
}

mappers::PortProvinces::PortProvinces(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PortProvinces::registerKeys()
{
	registerKeyword("allowlist", [this](const std::string& unused, std::istream& theStream) {
		const auto portVector = commonItems::intList(theStream).getInts();
		allowlist.insert(portVector.begin(), portVector.end());
	});
	registerKeyword("denylist", [this](const std::string& unused, std::istream& theStream) {
		auto portVector = commonItems::intList(theStream).getInts();
		denylist.insert(portVector.begin(), portVector.end());
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
