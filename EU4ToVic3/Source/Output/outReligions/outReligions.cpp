#include "outReligions.h"
#include "CommonFunctions.h"
#include <fstream>
#include <ranges>

namespace
{
void outReligion(std::ostream& output, const mappers::ReligionDef& religion)
{
	output << religion.name << " = {\n";
	if (!religion.texture.empty())
		output << "\ttexture = \"" << religion.texture << "\"\n";
	if (!religion.traits.empty())
	{
		output << "\ttraits = { ";
		for (const auto& trait: religion.traits)
			output << trait << " ";
		output << "}\n";
	}
	if (religion.color)
		output << "\tcolor " << *religion.color << "\n";
	if (!religion.taboos.empty())
	{
		output << "\ttaboos = { ";
		for (const auto& taboo: religion.taboos)
			output << taboo << " ";
		output << "}\n";
	}
	output << "}\n";
}

void outReligionStaticModifiers(std::ostream& output, const mappers::ReligionDef& religion)
{
	output << religion.name << "_standard_of_living_modifier_positive = {\n";
	output << "\ticon = \"gfx/interface/icons/timed_modifier_icons/modifier_flag_positive.dds\"\n";
	output << "\tstate_" << religion.name << "_standard_of_living_add = 1\n";
	output << "}\n";
	output << religion.name << "_standard_of_living_modifier_negative = {\n";
	output << "\ticon = \"gfx/interface/icons/timed_modifier_icons/modifier_flag_negative.dds\"\n";
	output << "\tstate_" << religion.name << "_standard_of_living_add = 1\n";
	output << "}\n\n";
}

void outReligionModifierTypeDefs(std::ostream& output, const mappers::ReligionDef& religion)
{
	output << "state_" << religion.name << "_standard_of_living_add = {\n";
	output << "\tdecimals=1\n";
	output << "\tcolor=good\n";
	output << "\tgame_data={\n";
	output << "\t\tai_value=0\n";
	output << "\t}\n";
	output << "}\n\n";
}
} // namespace

void OUT::exportReligions(const std::filesystem::path& outputName, const std::map<std::string, mappers::ReligionDef>& religions)
{
	std::ofstream outputReligion("output" / outputName / "common/religions/99_converted_religions.txt");
	if (!outputReligion.is_open())
		throw std::runtime_error("Could not create " + outputName.string() + "/common/religions/99_converted_religions.txt");
	std::ofstream outputReligionStatics("output" / outputName / "/common/static_modifiers/99_converted_religions_static_modifiers.txt");
	if (!outputReligionStatics.is_open())
		throw std::runtime_error("Could not create " + outputName.string() + "/common/static_modifiers/99_converted_religions_static_modifiers.txt");
	std::ofstream outputReligionModifierTypes("output" / outputName / "/common/modifier_type_definitions/99_converted_religions_modifier_type_defs.txt");
	if (!outputReligionModifierTypes.is_open())
		throw std::runtime_error("Could not create " + outputName.string() + "/common/modifier_type_definitions/99_converted_religions_modifier_type_defs.txt");

	outputReligion << commonItems::utf8BOM << "\n";
	outputReligionStatics << commonItems::utf8BOM << "\n";
	outputReligionModifierTypes << commonItems::utf8BOM << "\n";
	for (const auto& religion: religions | std::views::values)
	{
		outReligion(outputReligion, religion);
		outReligionStaticModifiers(outputReligionStatics, religion);
		outReligionModifierTypeDefs(outputReligionModifierTypes, religion);
	}
	outputReligion << "\n";
	outputReligion.close();
	outputReligionStatics << "\n";
	outputReligionStatics.close();
	outputReligionModifierTypes << "\n";
	outputReligionModifierTypes.close();
}
