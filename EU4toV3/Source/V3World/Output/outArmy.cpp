#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Army& army)
{
	if (army.regiments.empty())
	{
		return output;
	}
	if (army.isNavy)
	{
		output << "navy = {\n";
	}
	else
	{
		output << "army = {\n";
	}
	output << "\tname=\"" << army.name << "\"\n";
	output << "\tlocation=" << army.location << "\n";
	for (const auto& regiment: army.regiments)
	{
		output << regiment;
	}
	output << "}\n";
	output << "\n";

	return output;
}
