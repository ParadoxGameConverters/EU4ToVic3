#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Party& party)
{
	output << "\n";
	output << "party = {\n";
	output << "\tname = \"" << party.name << "\"\n";
	output << party.partyDetails;
	output << "}\n";
	return output;
}

std::ostream& mappers::operator<<(std::ostream& output, const PartyType& partyDetails)
{
	output << "\tstart_date = " << partyDetails.startDate << "\n";
	output << "\tend_date = " << partyDetails.endDate << "\n\n";
	output << "\tideology = " << partyDetails.ideology << "\n\n";
	for (const auto& [policy, position]: partyDetails.policies)
	{
		output << "\t" << policy << " = " << position << "\n";
	}
	return output;
}
