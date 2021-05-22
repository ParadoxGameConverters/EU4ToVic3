#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Regiment& regiment)
{
	if (regiment.isShip)
	{
		output << "\tship = {\n";
	}
	else
	{
		output << "\tregiment = {\n";
	}
	output << "\t\tname=\"" << regiment.name << "\"\n";
	output << "\t\ttype=" << RegimentTypeToName[regiment.regimentType] << "\n";
	if (!regiment.isShip)
	{
		output << "\t\thome=" << regiment.homeProvinceID << "\n";
	}
	output << "\t}\n";
	return output;
}
