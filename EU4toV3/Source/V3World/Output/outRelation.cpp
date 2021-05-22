#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Relation& relation)
{
	output << relation.target << " = {\n";
	output << "\tvalue = " << relation.relations << "\n";
	if (relation.militaryAccess)
	{
		output << "\tmilitary_access = yes\n";
	}
	if (relation.influence)
	{
		output << "\tinfluence_value = " << relation.influence << "\n";
	}
	output << "\tlevel = " << relation.level << "\n";
	output << "}\n";
	return output;
}
