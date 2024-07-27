#include "outBuildings.h"
#include "ClayManager/State/SubState.h"
#include "CommonFunctions.h"
#include "PoliticalManager/Country/Country.h"
#include <fstream>
#include <ranges>

namespace
{
void outPMs(std::ostream& output, const V3::Building& building)
{
	output << "\t\t\t\tactivate_production_methods = {";
	for (const auto& PM: building.getPMs())
	{
		output << " \"" << PM << "\"";
	}
	output << " }\n";
}
void outShareholders(std::ostream& output, const V3::Building& building)
{
	for (const auto& shareholder: building.getShareholders())
	{
		if (shareholder.level == 0)
		{
			Log(LogLevel::Warning) << shareholder.type << " ghost shareholder found in a lvl " << building.getLevel() << " " << building.getName()
										  << ". The shareholder is based in " << shareholder.tag << ": " << shareholder.state << ".";
		}

		output << "\t\t\t\tadd_ownership = {\n";
		if (shareholder.type == "national" || shareholder.type == "national_service")
		{
			output << "\t\t\t\t\tcountry = {\n";
			output << "\t\t\t\t\t\tcountry = c:" << shareholder.tag << "\n";
			output << "\t\t\t\t\t\tlevels = " << shareholder.level << "\n";
			output << "\t\t\t\t\t}\n";
		}
		if (shareholder.type == "local")
		{
			output << "\t\t\t\t\tbuilding = {\n";
			output << "\t\t\t\t\t\ttype = " << building.getName() << "\n";
			output << "\t\t\t\t\t\tcountry = c:" << shareholder.tag << "\n";
			output << "\t\t\t\t\t\tlevels = " << shareholder.level << "\n";
			output << "\t\t\t\t\t\tregion = " << shareholder.state << "\n";
			output << "\t\t\t\t\t}\n";
		}
		if (shareholder.type == "building_financial_district")
		{
			output << "\t\t\t\t\tbuilding = {\n";
			output << "\t\t\t\t\t\ttype = building_financial_district\n";
			output << "\t\t\t\t\t\tcountry = c:" << shareholder.tag << "\n";
			output << "\t\t\t\t\t\tlevels = " << shareholder.level << "\n";
			output << "\t\t\t\t\t\tregion = " << shareholder.state << "\n";
			output << "\t\t\t\t\t}\n";
		}
		if (shareholder.type == "building_manor_house")
		{
			output << "\t\t\t\t\tbuilding = {\n";
			output << "\t\t\t\t\t\ttype = building_manor_house\n";
			output << "\t\t\t\t\t\tcountry = c:" << shareholder.tag << "\n";
			output << "\t\t\t\t\t\tlevels = " << shareholder.level << "\n";
			output << "\t\t\t\t\t\tregion = " << shareholder.state << "\n";
			output << "\t\t\t\t\t}\n";
		}
		output << "\t\t\t\t}\n";
	}
}
void outBuilding(std::ostream& output, const V3::Building& building)
{
	output << "\t\t\tcreate_building = {\n";
	output << "\t\t\t\tbuilding = \"" << building.getName() << "\"\n";

	if (building.getShareholders().empty())
	{
		Log(LogLevel::Warning) << "Building " << building.getName() << " will be ignored by the game engine, no investor data.";
	}
	outShareholders(output, building);

	output << "\t\t\t\treserves = " << 1 << "\n";

	outPMs(output, building);

	output << "\t\t\t}\n";
}
void outSubStateBuildings(std::ostream& output, const V3::SubState& subState)
{
	if (!subState.getOwner())
		return;
	output << "\t\tregion_state:" << subState.getOwner()->getTag() << " = {\n";
	for (const auto& building: subState.getBuildings())
	{
		if (building->getLevel() > 0)
			outBuilding(output, *building);
	}
	for (const auto& element: subState.getVanillaBuildingElements())
		output << "\t\t\t" << element << "\n";
	output << "\t\t}\n";
}
void outStateBuildings(std::ostream& output, const V3::State& state)
{
	output << "\ts:" << state.getName() << " = {\n";
	for (const auto& substate: state.getSubStates())
	{
		if (substate->getBuildings().empty() && substate->getVanillaBuildingElements().empty())
			continue;
		outSubStateBuildings(output, *substate);
	}
	output << "\t}\n";
}
} // namespace

void OUT::exportBuildings(const std::string& outputName, const std::map<std::string, std::shared_ptr<V3::State>>& states)
{
	std::ofstream output("output/" + outputName + "/common/history/buildings/99_converted_buildings.txt");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/common/history/buildings/99_converted_buildings.txt");

	output << commonItems::utf8BOM << "BUILDINGS = {\n";
	for (const auto& state: states | std::views::values)
	{
		const auto& subs = state->getSubStates();
		if (std::all_of(subs.begin(), subs.end(), [](const auto& subState) {
				 return subState->getBuildings().empty() && subState->getVanillaBuildingElements().empty();
			 }))
		{
			continue;
		}

		outStateBuildings(output, *state);
	}
	output << "}\n";
	output.close();
}
