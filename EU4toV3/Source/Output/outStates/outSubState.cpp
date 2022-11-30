#include "outSubState.h"
#include <ostream>
#include <ranges>

void OUT::outSubState(std::ostream& output, const V3::SubState& substate)
{
	if (!substate.getOwnerTag())
		return; // Can't craft a state without an owner.

	output << "\t\tcreate_state = {\n";
	output << "\t\t\tcountry = c:" << *substate.getOwnerTag() << "\n";
	output << "\t\t\towned_provinces = { ";
	for (const auto& provinceID: substate.getProvinces() | std::views::keys)
		output << provinceID << " ";
	output << "}\n";

	// TODO: \t\t\t state_type = unincorporated
	// TODO: \t\t\t state_type = treaty_port (can be both)

	output << "\t\t}\n";
}
