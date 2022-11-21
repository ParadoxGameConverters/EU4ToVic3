#include "outCommonHistoryStates.h"
#include <ostream>
#include <ranges>

std::ostream& V3::outCommonHistoryStates(std::ostream& output, const std::map<std::string, std::shared_ptr<State>>& states)
{
	output << "STATES = {\n";
	for (const auto& state: states | std::views::values)
		if (!state->getSubStates().empty()) // states without substates aren't politically interesting.
			output << *state;
	output << "}\n";
	return output;
}