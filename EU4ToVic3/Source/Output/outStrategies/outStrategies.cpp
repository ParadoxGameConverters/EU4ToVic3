#include "outStrategies.h"
#include "CommonFunctions.h"
#include <fstream>
#include <ranges>

namespace
{
void outCountryStrategies(std::ostream& output, const std::map<std::string, int>& strategies)
{

	if (strategies.empty())
		return;
	if (strategies.size() == 1)
	{
		output << "\t\tset_strategy = " << strategies.begin()->first << "\n";
	}
	else
	{
		output << "\t\trandom_list = {\n";

		for (const auto& [strategy, value]: strategies)
			output << "\t\t\t" << value << " = { set_strategy = " << strategy << " }\n";

		output << "\t\t}\n";
	}
}

void outStrategies(std::ostream& output, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	output << "AI = {\n";
	for (const auto& country: countries | std::views::values)
	{
		if (country->getProcessedData().admStrategies.empty() && country->getProcessedData().dipStrategies.empty() &&
			 country->getProcessedData().polStrategies.empty())
			continue;

		output << "\tc:" << country->getTag() << " = {\n";
		outCountryStrategies(output, country->getProcessedData().admStrategies);
		outCountryStrategies(output, country->getProcessedData().dipStrategies);
		outCountryStrategies(output, country->getProcessedData().polStrategies);
		output << "\t}\n";
	}
	output << "}\n";
}
} // namespace

void OUT::exportStrategies(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream output("output/" + outputName + "/common/history/ai/99_converter_strategy.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/ai/99_converter_strategy.txt");

	output << commonItems::utf8BOM;
	outStrategies(output, countries);
	output.close();
}
