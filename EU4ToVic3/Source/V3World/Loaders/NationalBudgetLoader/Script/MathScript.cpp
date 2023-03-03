#include "MathScript.h"
#include "CommonRegexes.h"
#include "CountryManager/EU4Country.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "PoliticalManager/Country/Country.h"

void V3::MathScript::loadMathScript(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::MathScript::registerKeys()
{
	registerKeyword("value", [this](std::istream& theStream) {
		value = commonItems::getDouble(theStream);
	});
	registerRegex(commonItems::catchallRegex, [this](const std::string& triggerName, std::istream& theStream) {
		trigger = triggerName;
		triggerValue = commonItems::getString(theStream);
	});
}

bool V3::MathScript::isValid(const Country& country) const
{
	if (trigger == "is_colony")
	{
		return country.getSourceCountry()->isColony() == (triggerValue == "yes");
	}
	if (trigger == "is_trade_company")
	{
		return country.getSourceCountry()->isTradeCompany() == (triggerValue == "yes");
	}
	if (trigger == "vic3_law")
	{
		return country.getProcessedData().laws.contains(triggerValue);
	}
	if (trigger == "vic3_tech")
	{
		return country.getProcessedData().techs.contains(triggerValue);
	}
	if (trigger == "is_eu4_gp")
	{
		return country.getSourceCountry()->isGP() == (triggerValue == "yes");
	}
	if (trigger == "industry_score_less_than")
	{
		try
		{
			return country.getIndustryFactor() * 5 < std::stoi(triggerValue);
		}
		catch (const std::exception& e)
		{
			Log(LogLevel::Error) << "Failed to understand industry score " << triggerValue << ": " << e.what();
		}
	}

	Log(LogLevel::Error) << "Unknown trigger: " << trigger << " in MathScript.";
	return false;
}