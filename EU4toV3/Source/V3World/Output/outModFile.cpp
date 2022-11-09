#include "outModFile.h"

std::ostream& V3::outModFile(std::ostream& output, const std::string& outName)
{
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
		return output;
	}
}