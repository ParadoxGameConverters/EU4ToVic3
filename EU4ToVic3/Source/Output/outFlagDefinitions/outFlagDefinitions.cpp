#include "outFlagDefinitions.h"
#include "CommonFunctions.h"
#include "PoliticalManager/Country/Country.h"
#include <fstream>
#include <ranges>

namespace
{
void outFlagDefinitions(std::ostream& output, const std::string& tag, const V3::Country& country)
{
	output << tag << " = {\n";
	for (const auto& [flagType, coaCode]: country.getProcessedData().flags)
	{
		output << "\tflag_definition = {\n";
		output << "\t\tcoa = " << coaCode << "\n";
		output << "\t\tallow_overlord_canton = yes\n";

		if (flagType == V3::FlagCrafter::Default)
		{
			output << "\t\tsubject_canton = " << coaCode << "\n";
			output << "\t\tpriority = 1\n";
		}
		else if (flagType == V3::FlagCrafter::Republic)
		{
			output << "\t\tpriority = 10\n";
			output << "\t\tsubject_canton = " << coaCode << "\n";
			output << "\t\ttrigger = { coa_def_republic_flag_trigger = yes }\n";
		}
		else if (flagType == V3::FlagCrafter::Monarchy)
		{
			output << "\t\tpriority = 20\n";
			output << "\t\tsubject_canton = " << coaCode << "\n";
			output << "\t\toverlord_canton_scale = { 0.337 0.337 }\n";
			output << "\t\ttrigger = { coa_def_absolute_monarchy_flag_trigger = yes }\n";
		}
		else if (flagType == V3::FlagCrafter::Communist)
		{
			output << "\t\tpriority = 1500\n";
			output << "\t\ttrigger = { coa_def_communist_flag_trigger = yes }\n";
		}
		else if (flagType == V3::FlagCrafter::Fascist)
		{
			output << "\t\tpriority = 1500\n";
			output << "\t\tsubject_canton = " << coaCode << "\n";
			output << "\t\ttrigger = { coa_def_fascist_flag_trigger = yes }\n";
		}
		output << "\t}\n";
	}
	output << "}\n";
}
} // namespace

void OUT::exportFlagDefinitions(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream output("output/" + outputName + "/common/flag_definitions/99_converter_flag_definitions.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/flag_definitions/99_converter_flag_definitions.txt");
	output << commonItems::utf8BOM << "\n";

	for (const auto& [tag, country]: countries)
	{
		if (country->getProcessedData().flags.empty())
			continue;

		outFlagDefinitions(output, tag, *country);
	}
	output.close();
}
