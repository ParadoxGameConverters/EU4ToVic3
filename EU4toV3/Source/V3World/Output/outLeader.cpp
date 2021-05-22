#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Leader& leader)
{
	output << "leader = {\n";
	output << "\tname=\"" << leader.name << "\"\n";
	output << "\tdate=\"" << leader.activationDate << "\"\n";
	if (leader.isLand)
	{
		output << "\ttype=land\n";
	}
	else
	{
		output << "\ttype=sea\n";
	}
	output << "\tpersonality=\"" << leader.personality << "\"\n";
	output << "\tbackground=\"" << leader.background << "\"\n";
	output << "}\n";
	output << "\n";

	return output;
}
