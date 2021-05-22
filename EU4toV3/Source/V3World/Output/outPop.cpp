#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Pop& pop)
{
	if (pop.size <= 0) return output;
	
	output << "\t" << pop.type << "=\n";
	output << "\t{\n";
	output << "\t\tculture = " << pop.culture << "\n";
	output << "\t\treligion = " << pop.religion << "\n";
	output << "\t\tsize= " <<  pop.size << "\n";
	output << "\t}\n";
	output << "\n";
	return output;
}
