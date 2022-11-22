#include "outSubState.h"
#include <ostream>
#include <ranges>

void OUT::outSubState(std::ostream& output, const V3::SubState& substate)
{
	output << "\t\tcreate_state = {\n";
	output << "\t\t\tcountry = c:" << substate.ownerTag << "\n";
	output << "\t\t\towned_provinces = { ";
	for (const auto& provinceID: substate.provinces | std::views::keys)
		output << provinceID << " ";
	output << "}\n";

	// TODO: \t\t\t state_type = unincorporated
	// TODO: \t\t\t state_type = treaty_port (can be both)

	output << "\t\t}\n";
}
