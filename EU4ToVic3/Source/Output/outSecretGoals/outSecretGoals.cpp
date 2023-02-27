#include "outSecretGoals.h"
#include "CommonFunctions.h"
#include <fstream>
#include <ranges>

namespace
{
void outSecretGoals(std::ostream& output, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	output << "AI = {\n";
	for (const auto& country: countries | std::views::values)
	{
		if (country->getProcessedData().targetSecretGoals.empty())
			continue;

		output << "\tc:" << country->getTag() << " = {\n";
		for (const auto& [target, goal]: country->getProcessedData().targetSecretGoals)
		{
			output << "\t\tset_secret_goal = {\n";
			output << "\t\t\tcountry = c:" << target << "\n";
			output << "\t\t\tsecret_goal = " << goal << "\n";
			output << "\t\t}\n";
		}

		output << "\t}\n";
	}
	output << "}\n";
}
} // namespace

void OUT::exportSecretGoals(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	std::ofstream output("output/" + outputName + "/common/history/ai/99_converter_secret_goals.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/ai/99_converter_secret_goals.txt");

	output << commonItems::utf8BOM;
	outSecretGoals(output, countries);
	output.close();
}
