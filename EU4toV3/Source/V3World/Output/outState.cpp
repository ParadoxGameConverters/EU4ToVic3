#include "outState.h"
#include "ClayManager/SubState.h"
#include <ostream>

std::ostream& V3::operator<<(std::ostream& output, const State& state)
{
	output << "\ts:" << state.name << " = {\n";
	for (const auto& substate: state.substates)
		output << *substate;

	// TODO: \t\t add_homeland = culture
	// TODO: \t\t add_claim = c:TAG

	output << "\t}\n";
	return output;
}
