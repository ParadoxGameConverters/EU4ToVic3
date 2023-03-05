#include "ProductionMethod.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void V3::ProductionMethod::loadProductionMethod(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::ProductionMethod::registerKeys()
{
	registerKeyword("unlocking_technologies", [this](std::istream& theStream) {
		for (const auto& tech: commonItems::getStrings(theStream))
			unlockingTechs.emplace(tech);
	});
	registerKeyword("country_modifiers", [this](std::istream& theStream) {
		cModUnwrapper.parseStream(theStream);
	});
	registerKeyword("building_modifiers", [this](std::istream& theStream) {
		bModUnwrapper.parseStream(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	cModUnwrapper.registerKeyword("workforce_scaled", [this](std::istream& theStream) {
		const auto& theAssignments = commonItems::assignments(theStream).getAssignments();
		if (!theAssignments.contains("country_bureaucracy_add"))
		{
			return;
		}

		const std::string& bureaucracyString = theAssignments.at("country_bureaucracy_add");
		try
		{
			bureaucracy = std::stoi(bureaucracyString);
		}
		catch (const std::exception& e)
		{
			Log(LogLevel::Error) << "Failed to read bureaucracy output value " << bureaucracyString << ": " << e.what();
		}
	});
	cModUnwrapper.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	bModUnwrapper.registerKeyword("level_scaled", [this](std::istream& theStream) {
		const auto& theAssignments = commonItems::assignments(theStream).getAssignments();
		const std::regex pattern("building_employment_([a-zA-Z_]+)_add");

		for (const auto& [jobEffect, employmentNumber]: theAssignments)
		{
			std::smatch job;
			if (std::regex_search(jobEffect, job, pattern)) // building_employment_clerks_add -> clerks
			{
				try
				{
					employment[job[1].str()] = std::stoi(employmentNumber);
				}
				catch (const std::exception& e)
				{
					Log(LogLevel::Error) << "Failed to read employment number " << employmentNumber << ": " << e.what();
				}
			}
		}
	});
	bModUnwrapper.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
