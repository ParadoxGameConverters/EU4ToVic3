#include "outSubState.h"
#include <ostream>
#include <ranges>

std::ostream& V3::operator<<(std::ostream& output, const SubState& substate)
{
	// TODO: TEST OUTPUT
	output << "\t\tcreate_state = {\n";
	output << "\t\t\tcountry = c:" << substate.ownerTag << "\n";
	output << "\t\t\towned_provinces = { ";
	for (const auto& provinceID: substate.provinces | std::views::keys)
		output << provinceID << " ";
	output << "}\n";

	// TODO: \t\t\t state_type = unincorporated
	// TODO: \t\t\t state_type = treaty_port (can be both)

	output << "\t\t}\n";
	return output;
}
