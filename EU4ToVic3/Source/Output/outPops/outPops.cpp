#include "outPops.h"
#include "ClayManager/State/SubState.h"
#include "CommonFunctions.h"
#include "PoliticalManager/Country/Country.h"
#include <fstream>
#include <ranges>

namespace
{
void outPop(std::ostream& output, const V3::Pop& pop)
{
	if (pop.getSize() <= 0)
		return;
	output << "\t\t\tcreate_pop = {\n";
	if (!pop.getCulture().empty())
		output << "\t\t\t\tculture = " << pop.getCulture() << "\n";
	if (!pop.getReligion().empty())
		output << "\t\t\t\treligion = " << pop.getReligion() << "\n";
	if (!pop.getType().empty())
		output << "\t\t\t\tpop_type = " << pop.getType() << "\n";
	output << "\t\t\t\tsize = " << pop.getSize() << "\n";
	output << "\t\t\t}\n";
}
void outSubStatePops(std::ostream& output, const V3::SubState& subState)
{
	if (!subState.getOwner())
		return;
	output << "\t\tregion_state:" << subState.getOwner()->getTag() << " = {\n";
	for (const auto& pop: subState.getSubStatePops().getPops())
	{
		if (pop.getSize() == 0)
			continue;
		outPop(output, pop);
	}
	output << "\t\t}\n";
}

void outStatePops(std::ostream& output, const V3::State& state)
{
	output << "\ts:" << state.getName() << " = {\n";
	for (const auto& substate: state.getSubStates())
	{
		if (substate->getSubStatePops().getPopCount() == 0)
			continue;
		outSubStatePops(output, *substate);
	}
	output << "\t}\n";
}
} // namespace

void OUT::exportPops(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::State>>& states)
{
	std::ofstream output("output/" + outputName + "/common/history/pops/99_converted_pops.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/pops/99_converted_pops.txt");

	output << commonItems::utf8BOM << "POPS = {\n";
	for (const auto& state: states | std::views::values)
	{
		if (state->getStatePopCount() == 0)
			continue;
		outStatePops(output, *state);
	}
	output << "}\n";
	output.close();
}
