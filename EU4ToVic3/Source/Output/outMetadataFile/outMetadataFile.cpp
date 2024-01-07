#include "outMetadataFile.h"
#include "Log.h"
#include <fstream>

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
	output << "\t\t\"multiplayer_synchronized\" : true,\n";
	output << "\t\t\"replace_paths\" : [\n";
	output << "\t\t\t\"common/cultures\",\n";
	output << "\t\t\t\"common/country_definitions\",\n";
	output << "\t\t\t\"common/history/buildings\",\n";
	output << "\t\t\t\"common/history/characters\",\n";
	output << "\t\t\t\"common/history/countries\",\n";
	output << "\t\t\t\"common/history/diplomacy\",\n";
	output << "\t\t\t\"common/history/diplomatic_plays\",\n";
	output << "\t\t\t\"common/history/governments\",\n";
	output << "\t\t\t\"common/history/interests\",\n";
	output << "\t\t\t\"common/history/military_deployments\",\n";
	output << "\t\t\t\"common/history/military_formations\",\n";
	output << "\t\t\t\"common/history/pops\",\n";
	output << "\t\t\t\"common/history/population\",\n";
	output << "\t\t\t\"common/history/production_methods\",\n";
	output << "\t\t\t\"common/history/states\",\n";
	output << "\t\t\t\"common/history/trade_routes\",\n";
	output << "\t\t\t\"common/religions\"\n";
	output << "\t\t]\n";
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
