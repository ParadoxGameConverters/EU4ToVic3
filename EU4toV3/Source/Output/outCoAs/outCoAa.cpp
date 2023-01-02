#include "outCoAa.h"
#include "CommonFunctions.h"
#include "PoliticalManager/Country/Country.h"
#include <fstream>
#include <ranges>

namespace
{
void outCustomCoAs(std::ostream& output, const V3::Country& country)
{
	for (const auto& customFlag: country.getProcessedData().customFlags | std::views::values)
	{
		output << customFlag << " = { textured_emblem = { texture = \"custom_export/" << customFlag << ".tga\" } }\n";
	}
}
} // namespace

void OUT::exportCustomCoAs(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream output("output/" + outputName + "/common/coat_of_arms/coat_of_arms/99_custom_coas.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/coat_of_arms/coat_of_arms/99_custom_coas.txt");
	output << commonItems::utf8BOM << "\n";

	for (const auto& country: countries | std::views::values)
	{
		if (country->getProcessedData().customFlags.empty())
			continue;

		outCustomCoAs(output, *country);
	}
	output.close();
}
