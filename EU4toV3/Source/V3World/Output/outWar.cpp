#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const War& war)
{
	output << "name = \"" << war.name << "\"\n";
	output << "\n";
	output << war.details.startDate << " = {\n";
	for (const auto& actor: war.attackers)
	{
		output << "\tadd_attacker = " << actor << "\n";
	}
	for (const auto& actor : war.defenders)
	{
		output << "\tadd_defender = " << actor << "\n";
	}
	output << "\n";
	output << "\twar_goal = {\n";
	output << "\t\tcasus_belli = " << war.details.warGoalType << "\n";
	output << "\t\tactor = " << war.attackers[0] << "\n";
	output << "\t\treceiver = " << war.defenders[0] << "\n";
	if (!war.details.targetTag.empty())
	{
		output << "\t\tcountry = " << war.details.targetTag << "\n";
	}
	if (war.details.targetProvinceID)
	{
		output << "\t\tstate_province_id = " << war.details.targetProvinceID << "\n";
	}
	output << "\t}\n";
	output << "}\n";
	return output;
}
