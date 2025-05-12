#include "outCommonHistoryStates.h"
#include "ClayManager/State/SubState.h"
#include "CommonFunctions.h"
#include <fstream>
#include <ranges>

namespace
{
void outSubState(std::ostream& output, const V3::SubState& substate)
{
	if (!substate.getOwnerTag())
		return; // Can't craft a state without an owner.

	output << "\t\tcreate_state = {\n";
	output << "\t\t\tcountry = c:" << *substate.getOwnerTag() << "\n";
	output << "\t\t\towned_provinces = { ";
	for (const auto& provinceID: substate.getProvinces() | std::views::keys)
		output << provinceID << " ";
	output << "}\n";
	for (const auto& stateType: substate.getSubStateTypes())
		output << "\t\t\tstate_type = " << stateType << "\n";

	output << "\t\t}\n";
}

void outState(std::ostream& output, const V3::State& state)
{
	output << "\ts:" << state.getName() << " = {\n";
	for (const auto& substate: state.getSubStates())
		outSubState(output, *substate);
	for (const auto& homeLand: state.getHomelands())
		output << "\t\tadd_homeland = cu:" << homeLand << "\n";
	for (const auto& tag: state.getClaims())
		output << "\t\tadd_claim = c:" << tag << "\n";

	output << "\t}\n";
}

void outCommonHistoryStates(std::ostream& output, const std::map<std::string, std::shared_ptr<V3::State>>& states)
{
	output << "STATES = {\n";
	for (const auto& state: states | std::views::values)
		if (!state->getSubStates().empty()) // states without substates aren't politically interesting.
			outState(output, *state);
	output << "}\n";
}
} // namespace

void OUT::exportCommonHistoryStates(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::State>>& states)
{
	std::ofstream output("output/" + outputName + "/common/history/states/99_converter_states.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/states/99_converter_states.txt");

	output << commonItems::utf8BOM;
	outCommonHistoryStates(output, states);
	output.close();
}
