#include "outModFile.h"

std::ostream& V3::outModFile(std::ostream& output, const std::string& outName)
{
	output << "name = \"Converted - " << outName << "\"\n";
	output << "path = \"mod/" << outName << "\"\n";
	output << "supported_version = \"1.0.*\"\n";
	output << "replace_path = \"history/countries/\"\n";
	output << "replace_path = \"history/provinces/\"\n";
	output << "replace_path = \"history/diplomacy/\"\n";
	output << "replace_path = \"history/wars/\"\n";
	output << "replace_path = \"common/countries/\"\n";
	output << "replace_path = \"common/country_tags/\"\n";
	output << "replace_path = \"common/bookmarks/\"\n";
	return output;
}
