#include "PartyTypeMapper.h"
#include "ParserHelpers.h"
#include <Log.h>
#include "CommonRegexes.h"

mappers::PartyTypeMapper::PartyTypeMapper()
{
	Log(LogLevel::Info) << "Parsing party detail blobs.";
	registerKeys();
	parseFile("configurables/party_blobs.txt");
	clearRegisteredKeywords();
}

mappers::PartyTypeMapper::PartyTypeMapper(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::PartyTypeMapper::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& partyIdeology, std::istream& theStream) {
		PartyType newType(theStream);
		newType.setIdeology(partyIdeology);
		partyTypeMap.insert(std::make_pair(partyIdeology, newType));
	});
}

std::optional<mappers::PartyType> mappers::PartyTypeMapper::getPartyTypeByIdeology(const std::string& ideology) const
{
	if (const auto& partyTypeItr = partyTypeMap.find(ideology); partyTypeItr != partyTypeMap.end())
		return partyTypeItr->second;
	else
		return std::nullopt;
}
