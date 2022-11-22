#include "outCommonHistoryStates.h"
#include "CommonFunctions.h"
#include "outState.h"
#include <fstream>
#include <ranges>

void OUT::exportCommonHistoryStates(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::State>>& states)
{
	std::ofstream output("output/" + outputName + "/common/history/states/99_converter_states.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/states/99_converter_states.txt");

	output << commonItems::utf8BOM;
	outCommonHistoryStates(output, states);
	output.close();
}

std::ostream& OUT::outCommonHistoryStates(std::ostream& output, const std::map<std::string, std::shared_ptr<V3::State>>& states)
{
	output << "STATES = {\n";
	for (const auto& state: states | std::views::values)
		if (!state->getSubStates().empty()) // states without substates aren't politically interesting.
			outState(output, *state);
	output << "}\n";
	return output;
}