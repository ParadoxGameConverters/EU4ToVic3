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
} // namespace

void OUT::exportReligions(const std::string& outputName, const std::map<std::string, mappers::ReligionDef>& religions)
{
	std::ofstream output("output/" + outputName + "/common/religions/99_converted_religions.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/religions/99_converted_religions.txt");

	output << commonItems::utf8BOM << "\n";
	for (const auto& religion: religions | std::views::values)
	{
		outReligion(output, religion);
	}
	output << "\n";
	output.close();
}
