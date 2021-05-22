#include "Configuration.h"
#include "Country/Country.h"
#include "V3World.h"
#include "Log.h"
#include <fstream>

void V2::World::outputGTFO(std::map<std::string, std::shared_ptr<V2::Country>> countries) const
{
	const std::set<std::string> skip = {"AAA", "REB", "ALL", "CAT", "NAP", "YUE", "HRE", "MIN", "YAO"};

	std::ofstream outLoc("output/" + theConfiguration.getOutputName() + "/localisation/02_NNM_gtfo.csv");
	if (!outLoc.is_open())
		Log(LogLevel::Debug) << "Could not open 02_NNM_gtfo.csv for writing!";
	outLoc << ";;;;;;;;;;;;x,,,,,,,,,,,,,\n";
	outLoc << "gtfo_on_title;Enable 'Release Vassal' decisions;;;;;;;;;;;;x,,\n";
	outLoc << "gtfo_on_desc;The Release Vassal decisions allow you to release vassals, and are otherwise kept hidden from view to avoid having them clutter up your decision list.;;;;;;;;;;;;x\n";
	outLoc << "gtfo_off_title;Disable 'Release Vassal' decisions;;;;;;;;;;;;x,,\n";
	outLoc << "gtfo_off_desc;Your decision list will now be uncluttered!;;;;;;;;;;;;x,,\n";
	outLoc << "gtfo_disabled_title;Deactivate 'Release Vassal' decisions;;;;;;;;;;;;x,,\n";
	outLoc << "gtfo_disabled_desc;The 'Release Vassal' decisions will disappear from this game session and won't reappear again. You won't be able to use these decisions again for this game.;;;;;;;;;;;;x,,\n";

	std::ofstream output("output/" + theConfiguration.getOutputName() + "/decisions/gtfo.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not open gtfo.txt for writing!");
	output << "political_decisions = {\n";
	output << "\tgtfo_on = {\n";
	output << "\t\tpicture = gtfo\n";
	output << "\t\talert = no\n";
	output << "\t\tpotential = {\n";
	output << "\t\t\tOR = {\n";
	output << "\t\t\t\thas_country_flag = government_decisions_on\n";
	output << "\t\t\t\tai = yes\n";
	output << "\t\t\t}\n";
	output << "\t\t\tnum_of_vassals_no_substates = 1\n";
	output << "\t\t\tis_vassal = no\n";
	output << "\t\t\tNOT = { has_country_flag = gtfo has_country_flag = gtfo_disabled }\n";
	output << "\t\t}\n";
	output << "\t\tallow = { civilized = yes ai = no }\n";
	output << "\t\teffect = { set_country_flag = gtfo }\n";
	output << "\t}\n";
	output << "\tgtfo_off = {\n";
	output << "\t\tpicture = gtfo\n";
	output << "\t\talert = no\n";
	output << "\t\tpotential = {\n";
	output << "\t\t\thas_country_flag = gtfo\n";
	output << "\t\t\tNOT = { has_country_flag = gtfo_disabled }\n";
	output << "\t\t}\n";
	output << "\t\tallow = { ai = no }\n";
	output << "\t\teffect = { clr_country_flag = gtfo }\n";
	output << "\t}\n";
	output << "\tgtfo_disabled = {\n";
	output << "\t\tpicture = gtfo\n";
	output << "\t\talert = no\n";
	output << "\t\tpotential = {\n";
	output << "\t\t\tOR = {\n";
	output << "\t\t\t\thas_country_flag = government_decisions_on\n";
	output << "\t\t\t\tai = yes\n";
	output << "\t\t\t}\n";
	output << "\t\t\thas_country_flag = gtfo\n";
	output << "\t\t\tNOT = { has_country_flag = gtfo_disabled }\n";
	output << "\t\t}\n";
	output << "\t\tallow = { ai = no }\n";
	output << "\t\teffect = { clr_country_flag = gtfo set_country_flag = gtfo_disabled }\n";
	output << "\t}\n";

	for (const auto& [tag, country]: countries)
	{
		if (skip.contains(tag))
			continue;
		std::string countryName;
		if (!country->getLocalName().empty())
		{
			countryName = country->getLocalName();
		}
		else
		{
			const auto& commonCountryFile = country->getCommonCountryFile();
			const auto& dotPos = commonCountryFile.find_first_of('.');
			countryName = commonCountryFile.substr(0, dotPos);
		}

		output << "\n";
		output << "\tgtfo_" << tag << " = {\n";
		output << "\t\tpicture = gtfo\n";
		output << "\t\talert = no\n";
		output << "\n";
		output << "\t\tpotential = {\n";
		output << "\t\t\tai = no\n";
		output << "\t\t\thas_country_flag = gtfo\n";
		output << "\t\t\t" << tag << " = {\n";
		output << "\t\t\t\tvassal_of = THIS\n";
		output << "\t\t\t\tNOT = { substate_of = THIS }\n";
		output << "\t\t\t}\n";
		output << "\t\t}\n";
		output << "\n";
		output << "\t\tallow = {\n";
		output << "\t\t\twar = no\n";
		output << "\t\t\t" << tag << " = { war = no }\n";
		output << "\t\t}\n";
		output << "\n";
		output << "\t\teffect = {\n";
		output << "\t\t\tprestige_factor = -0.1\n";
		output << "\t\t\trelease_vassal = " << tag << "\n";
		output << "\t\t\tdiplomatic_influence = { who = " << tag << " value = -100 }\n";
		output << "\t\t}\n";
		output << "\t}\n";

		outLoc << "gtfo_" << tag << "_title;Grant " << countryName << " Independence;;;;;;;;;;;;x,,\n";
		outLoc << "gtfo_" << tag << "_desc;We've had a good run, but it's time to end our rulership of " << countryName
				 << ": if you love something, let it go!;;;;;;;;;;;;x\n";
	}

	output << "}\n";
	output.close();
}

void V2::World::outputReturnCores(std::map<std::string, std::shared_ptr<V2::Country>> countries) const
{
	const std::set<std::string> skip = {"AAA", "REB", "ALL", "CAT", "NAP", "YUE", "HRE", "MIN", "YAO"};

	std::ofstream outLoc("output/" + theConfiguration.getOutputName() + "/localisation/00_HPM_return_cores.csv");
	if (!outLoc.is_open())
		Log(LogLevel::Debug) << "Could not open 00_HPM_return_cores.csv for writing!";
	outLoc << ";;;;;;;;;;;;x,,,,,,,,,,,,,\n";
	outLoc << "return_core_on_title;Enable 'Return Cores' decisions;;;;;;;;;;;;x,,\n";
	outLoc << "return_core_on_desc;The Return Cores decisions allow you to return cores of vassals and spherelings, as long as you don't have a core in the province, and are otherwise kept hidden from view to avoid having them clutter up your decision list.;;;;;;;;;;;;x\n";
	outLoc << "return_core_off_title;Disable 'Return Cores' decisions;;;;;;;;;;;;x,,\n";
	outLoc << "return_core_off_desc;Your decision list will now be uncluttered.;;;;;;;;;;;;x,,\n";
	outLoc << "return_core_disabled_title;Deactivate 'Return Cores' decisions;;;;;;;;;;;;x,,\n";
	outLoc << "return_core_disabled_desc;The 'Return Cores' decisions will disappear from this game session and won't reappear again. You won't be able to use these decisions again for this game.;;;;;;;;;;;;x,,\n";

	std::ofstream output("output/" + theConfiguration.getOutputName() + "/decisions/Return_cores.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not open Return_cores.txt for writing!");
	output << "political_decisions = {\n";
	output << "\treturn_core_on = {\n";
	output << "\t\tpicture = return_cores_img\n";
	output << "\t\talert = no\n";
	output << "\t\tpotential = {\n";
	output << "\t\t\tOR = {\n";
	output << "\t\t\t\thas_country_flag = government_decisions_on\n";
	output << "\t\t\t\tai = yes\n";
	output << "\t\t\t}\n";
	output << "\t\t\tOR = {\n";
	output << "\t\t\t\tnum_of_vassals_no_substates = 1\n";
	output << "\t\t\t\tis_greater_power = yes\n";
	output << "\t\t\t}\n";
	output << "\t\t\tNOT = {\n";
	output << "\t\t\t\thas_country_flag = return_cores_decision\n";
	output << "\t\t\t\thas_country_flag = return_core_disabled \n";
	output << "\t\t\t}\n";
	output << "\t\t\tany_owned_province = { NOT = { is_core = THIS } any_core = { OR = { in_sphere = THIS vassal_of = THIS } } } \n";
	output << "\t\t}\n";
	output << "\t\tallow = { civilized = yes ai = no }\n";
	output << "\t\teffect = { set_country_flag = return_cores_decision } \n";
	output << "\t}\n";
	output << "\treturn_core_off = { \n";
	output << "\t\tpicture = return_cores_img\n";
	output << "\t\talert = no \n";
	output << "\t\tpotential = {\n";
	output << "\t\t\thas_country_flag = return_cores_decision \n";
	output << "\t\t\tNOT = { has_country_flag = return_core_disabled_forever } \n";
	output << "\t\t}\n";
	output << "\t\tallow = { ai = no }\n";
	output << "\t\teffect = { clr_country_flag = return_cores_decision } \n";
	output << "\t}\n";
	output << "\treturn_core_disabled = {\n";
	output << "\t\tpicture = return_cores_img\n";
	output << "\t\talert = no\n";
	output << "\t\tpotential = {\n";
	output << "\t\t\tOR = {\n";
	output << "\t\t\t\thas_country_flag = government_decisions_on\n";
	output << "\t\t\t\tai = yes\n";
	output << "\t\t\t}\n";
	output << "\t\t\thas_country_flag = return_cores_decision \n";
	output << "\t\t\tNOT = { has_country_flag = return_core_disabled }\n";
	output << "\t\t}\n";
	output << "\t\tallow = { ai = no }\n";
	output << "\t\teffect = { clr_country_flag = return_cores_decision set_country_flag = return_core_disabled } \n";
	output << "\t}\n";

	for (const auto& [tag, country]: countries)
	{
		if (skip.contains(tag))
			continue;
		std::string countryName;
		if (!country->getLocalName().empty())
		{
			countryName = country->getLocalName();
		}
		else
		{
			const auto& commonCountryFile = country->getCommonCountryFile();
			const auto& dotPos = commonCountryFile.find_first_of('.');
			countryName = commonCountryFile.substr(0, dotPos);
		}
		
		output << "\n";
		output << "\treturn_cores_" << tag << " = {\n";
		output << "\t\tpicture = return_cores_img\n";
		output << "\t\talert = no\n";
		output << "\n";
		output << "\t\tpotential = {\n";
		output << "\t\t\tai = no\n";
		output << "\t\t\thas_country_flag = return_cores_decision\n";
		output << "\t\t\t" << tag << " = {\n";
		output << "\t\t\t\tNOT = { substate_of = THIS }\n";
		output << "\t\t\t\tOR = {\n";
		output << "\t\t\t\t\tin_sphere = THIS\n";
		output << "\t\t\t\t\tvassal_of = THIS\n";
		output << "\t\t\t\t}\n";
		output << "\t\t\t}\n";
		output << "\t\t\tany_owned_province = {\n";
		output << "\t\t\t\tis_core = " << tag << "\n";
		output << "\t\t\t\tNOT = { is_core = THIS }\n";
		output << "\t\t\t}\n";
		output << "\t\t}\n";
		output << "\n";
		output << "\t\tallow = {\n";
		output << "\t\t\twar = no\n";
		output << "\t\t\t" << tag << " = { war = no }\n";
		output << "\t\t}\n";
		output << "\n";
		output << "\t\teffect = {\n";
		output << "\t\t\tprestige = -5\n";
		output << "\t\t\trelation = { who = " << tag << " value = 50 }\n";
		output << "\t\t\tany_owned = {\n";
		output << "\t\t\t\tlimit = {\n";
		output << "\t\t\t\t\tNOT = { is_core = THIS }\n";
		output << "\t\t\t\t\tis_core = " << tag << "\n";
		output << "\t\t\t\t}\n";
		output << "\t\t\t\tsecede_province = " << tag << "\n";
		output << "\t\t\t}\n";
		output << "\t\t}\n";
		output << "\t}\n";

		outLoc << "return_cores_" << tag << "_title;Return cores to " << countryName << ";;;;;;;;;;;;;;;;;;;;x,,\n";
		outLoc << "return_cores_" << tag << "_desc;We hold provinces that " << countryName
				 << " has claims on. Returning these provinces, as an act of good faith, would surely bolster our relations.;;;;;;;;;;;;x\n";
	}

	output << "}\n";
	output.close();
}