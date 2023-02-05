#include "outDiplomaticPlays/outDiplomaticPlays.h"
#include "CommonFunctions.h"
#include "PoliticalManager/Country/Country.h"
#include <fstream>
#include <ranges>

void OUT::exportDiplomaticPlays(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::Country>>& countries)
{
	for (const auto& [tag, country]: countries)
	{
		if (country->getProcessedData().vanillaDiplomaticPlayElements.empty())
			continue;

		std::ofstream output("output/" + outputName + "/common/history/diplomatic_plays/00_" + tag + ".txt");
		if (!output.is_open())
			throw std::runtime_error("Could not create " + outputName + "/common/history/diplomatic_plays/00_" + tag + ".txt");

		output << commonItems::utf8BOM << "DIPLOMATIC_PLAYS = {\n";
		output << "\tc:" << tag << " = {\n";
		for (const auto& element: country->getProcessedData().vanillaDiplomaticPlayElements)
		{
			output << "\t\t" << element << "\n";
		}
		output << "\t}\n";
		output << "}\n";
		output.close();
	}
}
