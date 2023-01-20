#include "outMetadataFile.h"
#include "Log.h"
#include <fstream>
#include "CommonFunctions.h"

namespace
{
void outMetadataFile(std::ostream& output, const std::string& outName)
{
	output << "{\n";
	output << "\t\"name\" : \"Converted - " << outName << "\",\n";
	output << "\t\"id\" : \"\",\n";
	output << "\t\"version\" : \"\",\n";
	output << "\t\"picture\" : \"thumbnail.png\",\n";
	output << "\t\"supported_game_version\" : \"\",\n";
	output << "\t\"short_description\" : \"Conversion from EU4: " << outName << "\",\n";
	output << "\t\"tags\" : [\"Alternative History\", \"Conversion\"],\n";
	output << "\t\"relationships\" : [],\n";
	output << "\t\"game_custom_data\" : {\n";
	output << "\t\t\"multiplayer_synchronized\" : true\n";
	output << "\t}\n";
	output << "}\n";
}
} // namespace

void OUT::exportMetadataFile(const std::string& outputName)
{
	std::ofstream output("output/" + outputName + "/.metadata/metadata.json");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/.metadata/metadata.json");
	Log(LogLevel::Info) << "<< Writing to: output/" + outputName + "/.metadata/metadata.json";
	outMetadataFile(output, outputName);
	output.close();
}
