#include "outConverterVersion.h"
#include <fstream>

std::ostream& mappers::operator<<(std::ostream& output, const ConverterVersion& version)
{
	output << "\n\n";
	output << "************ -= The Paradox Converters Team =- ********************\n";
	output << "* Converter version " << version.version << " \"" << version.name << "\"\n";
	output << "* " << version.descriptionLine << "\n";
	output << "* Built on " << __TIMESTAMP__ << "\n";
	output << "********************** + EU4 To Vic3 + ****************************\n";
	return output;
}