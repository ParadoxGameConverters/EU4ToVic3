#include "outCultures.h"
#include "CommonFunctions.h"
#include <fstream>
#include <ranges>

namespace
{
void outCulture(std::ostream& output, const mappers::CultureDef& culture)
{
	output << culture.name << " = {\n";
	if (culture.color)
		output << "\tcolor " << *culture.color << "\n";
	if (!culture.religion.empty())
		output << "\treligion = \"" << culture.religion << "\"\n";
	if (!culture.traits.empty())
	{
		output << "\ttraits = { ";
		for (const auto& trait: culture.traits)
			output << trait << " ";
		output << "}\n";
	}
	if (!culture.maleCommonFirstNames.empty())
	{
		output << "\tmale_common_first_names = { ";
		for (const auto& name: culture.maleCommonFirstNames)
			output << name << " ";
		output << "}\n";
	}
	if (!culture.femaleCommonFirstNames.empty())
	{
		output << "\tfemale_common_first_names = { ";
		for (const auto& name: culture.femaleCommonFirstNames)
			output << name << " ";
		output << "}\n";
	}
	if (!culture.nobleLastNames.empty())
	{
		output << "\tnoble_last_names = { ";
		for (const auto& name: culture.nobleLastNames)
			output << name << " ";
		output << "}\n";
	}
	if (!culture.commonLastNames.empty())
	{
		output << "\tcommon_last_names = { ";
		for (const auto& name: culture.commonLastNames)
			output << name << " ";
		output << "}\n";
	}
	if (!culture.maleRegalFirstNames.empty())
	{
		output << "\tmale_regal_first_names = { ";
		for (const auto& name: culture.maleRegalFirstNames)
			output << name << " ";
		output << "}\n";
	}
	if (!culture.femaleRegalFirstNames.empty())
	{
		output << "\tfemale_regal_first_names = { ";
		for (const auto& name: culture.femaleRegalFirstNames)
			output << name << " ";
		output << "}\n";
	}
	if (!culture.regalLastNames.empty())
	{
		output << "\tregal_last_names = { ";
		for (const auto& name: culture.regalLastNames)
			output << name << " ";
		output << "}\n";
	}
	if (!culture.ethnicities.empty())
	{
		output << "\tethnicities = {\n";
		auto counter = 1;
		for (const auto& ethnicity: culture.ethnicities)
			output << "\t\t" << counter++ << " = " << ethnicity << "\n";
		output << "\t}\n";
	}
	if (!culture.graphics.empty())
		output << "\tgraphics = " << culture.graphics << "\n";
	output << "}\n\n";
}
} // namespace

void OUT::exportCultures(const std::string& outputName, const std::map<std::string, mappers::CultureDef>& cultures)
{
	std::ofstream output("output/" + outputName + "/common/cultures/99_converted_cultures.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/religions/99_converted_cultures.txt");

	output << commonItems::utf8BOM << "\n";
	for (const auto& culture: cultures | std::views::values)
	{
		if (culture.skipExport)
			continue;
		outCulture(output, culture);
	}
	output << "\n";
	output.close();

	// Copy over Decentralized World discrimination traits
	auto files = commonItems::GetAllFilesInFolder("configurables/decentralized_world/common/discrimination_traits/");
	for (const auto& file: files)
		commonItems::TryCopyFile("configurables/decentralized_world/common/discrimination_traits/" + file,
			 "output/" + outputName + "/common/discrimination_traits/" + file);
}
