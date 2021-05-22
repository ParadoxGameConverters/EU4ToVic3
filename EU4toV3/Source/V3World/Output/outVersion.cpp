#include "output.h"

std::ostream& mappers::operator<<(std::ostream& output, const VersionParser& versionParser)
{
	output << "\n\n";
	output << "************ -= The Paradox Converters Team =- ********************\n";
	output << "* Converter version " << versionParser.version << " \"" << versionParser.name << "\"\n";
	output << "* " << versionParser.descriptionLine << "\n";
	output << "* Built on " << __TIMESTAMP__ << "\n";
	output << "********************** + EU4 To Vic3 + ****************************\n";
	return output;
}
