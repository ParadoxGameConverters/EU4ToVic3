#include "output.h"

std::ostream& V2::operator<<(std::ostream& output, const Agreement& agreement)
{
	output << agreement.type << "=\n";
	output << "{\n";
	output << "\tfirst=\"" << agreement.first << "\"\n";
	output << "\tsecond=\"" << agreement.second << "\"\n";
	output << "\tstart_date=\"" << agreement.start_date << "\"\n";
	output << "\tend_date=\"1936.1.1\"\n";
	output << "}\n";
	output << "\n";
	return output;
}
