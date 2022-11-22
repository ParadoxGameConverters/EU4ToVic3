#include "outState.h"
#include "outSubState.h"
#include <ostream>

void OUT::outState(std::ostream& output, const V3::State& state)
{
	output << "\ts:" << state.getName() << " = {\n";
	for (const auto& substate: state.getSubStates())
		outSubState(output, *substate);

	// TODO: \t\t add_homeland = culture
	// TODO: \t\t add_claim = c:TAG

	output << "\t}\n";
}
