#include "DiplomaticMapper.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void mappers::DiplomaticMapper::loadMappingRules(const std::string& filePath)
{
	Log(LogLevel::Info) << "-> Parsing Diplomatic Mapping Rules.";
	registerKeys();
	parseFile(filePath);
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> Loaded " << agreementTypes.size() + relationshipBoosts.size() << " categories.";
}

void mappers::DiplomaticMapper::registerKeys()
{
	registerRegex(R"([-+\d]+)", [this](const std::string& boostValue, std::istream& theStream) {
		try
		{
			int boost = std::stoi(boostValue);
			const auto theList = commonItems::getStrings(theStream);
			relationshipBoosts.emplace(boost, std::set<std::string>{});
			relationshipBoosts.at(boost).insert(theList.begin(), theList.end());
		}
		catch (std::exception& e)
		{
			Log(LogLevel::Error) << "Broken boost value inside diplomatic map: " << boostValue << " - " << e.what();
		}
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& agreementType, std::istream& theStream) {
		const auto theList = commonItems::getStrings(theStream);
		agreementTypes.emplace(agreementType, std::set<std::string>{});
		agreementTypes.at(agreementType).insert(theList.begin(), theList.end());
	});
}

std::optional<std::string> mappers::DiplomaticMapper::getAgreementType(const std::string& agreement) const
{
	for (const auto& [type, agreements]: agreementTypes)
		if (agreements.contains(agreement))
			return type;
	return std::nullopt;
}

int mappers::DiplomaticMapper::getRelationshipBoost(const std::string& agreement) const
{
	for (const auto& [boost, agreements]: relationshipBoosts)
		if (agreements.contains(agreement))
			return boost;
	return 0;
}
