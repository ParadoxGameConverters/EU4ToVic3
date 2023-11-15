#include "MajorFormablesEntry.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

V3::MajorFormablesEntry::MajorFormablesEntry(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::MajorFormablesEntry::registerKeys()
{
	registerKeyword("possible", [this](std::istream& theStream) {
		possibleParser.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& stanzaKey, std::istream& theStream) {
		stanzas.emplace_back(stanzaKey + " " + commonItems::stringOfItem(theStream).getString());
	});

	possibleParser.registerKeyword("OR", [this](std::istream& theStream) {
		orParser.parseStream(theStream);
	});
	possibleParser.registerKeyword("any_country", [this](std::istream& theStream) {
		anyCountryParser.parseStream(theStream);
	});
	possibleParser.registerRegex(commonItems::catchallRegex, [this](const std::string& stanzaKey, std::istream& theStream) {
		possibleStanzas.emplace_back(stanzaKey + " " + commonItems::stringOfItem(theStream).getString());
	});

	anyCountryParser.registerKeyword("OR", [this](std::istream& theStream) {
		orParser.parseStream(theStream);
	});
	anyCountryParser.registerKeyword("filter", [this](std::istream& theStream) {
		filterParser.parseStream(theStream);
	});
	anyCountryParser.registerKeyword("has_technology_researched", [this](std::istream& theStream) {
		requiredTechnology = commonItems::getString(theStream);
	});
	anyCountryParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	filterParser.registerKeyword("OR", [this](std::istream& theStream) {
		orParser.parseStream(theStream);
	});
	filterParser.registerKeyword("has_technology_researched", [this](std::istream& theStream) {
		requiredTechnology = commonItems::getString(theStream);
	});
	filterParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	orParser.registerKeyword("country_has_primary_culture", [this](std::istream& theStream) {
		auto culture = commonItems::getString(theStream);
		if (culture.starts_with("cu:") && culture.size() > 3)
			culture = culture.substr(3, culture.size());
		else
			Log(LogLevel::Warning) << "Major formable culture " << culture << " is malformed! It should be cu:<culture>!";
		cultures.emplace(culture);
	});
	orParser.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
