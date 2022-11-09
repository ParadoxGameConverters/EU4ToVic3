#include "V3World.h"
#include "../EU4World/World.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "Output/outModFile.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;

V3::World::World(const Configuration& configuration, const EU4::World& sourceWorld, commonItems::ConverterVersion&& theConverterVersion):
	 V3Path(configuration.getVic3Path()), configBlock(configuration.configBlock), converterVersion(std::move(theConverterVersion)),
	 outputName(configuration.getOutputName())
{
	Log(LogLevel::Progress) << "45 %";

	Log(LogLevel::Info) << "*** Hello Vicky 3, creating world. ***";
	Log(LogLevel::Info) << "-> Importing Provinces";
	Log(LogLevel::Progress) << "46 %";

	Log(LogLevel::Info) << "-> Importing Vanilla Pops";
	Log(LogLevel::Progress) << "47 %";

	Log(LogLevel::Info) << "-> Importing Potential Countries";
	Log(LogLevel::Progress) << "48 %";

	Log(LogLevel::Info) << "-> Loading Country Mapping Rules";
	Log(LogLevel::Progress) << "49 %";

	Log(LogLevel::Info) << "-> Loading Culture Mapping Rules";
	Log(LogLevel::Progress) << "50 %";

	Log(LogLevel::Info) << "-> Pouring From Hollow Into Empty";
	Log(LogLevel::Progress) << "51 %";

	Log(LogLevel::Info) << "-> Converting Countries";
	Log(LogLevel::Progress) << "52 %";

	Log(LogLevel::Info) << "-> Converting Provinces";
	Log(LogLevel::Progress) << "53 %";

	Log(LogLevel::Info) << "-> Cataloguing Invasive Fauna";
	Log(LogLevel::Progress) << "54 %";

	Log(LogLevel::Info) << "-> Converting Diplomacy";
	Log(LogLevel::Progress) << "55 %";

	Log(LogLevel::Info) << "-> Setting Up States";
	Log(LogLevel::Progress) << "57 %";

	Log(LogLevel::Info) << "-> Generating Unciv Reforms";
	Log(LogLevel::Progress) << "58 %";

	Log(LogLevel::Info) << "-> Converting Technology Levels";
	Log(LogLevel::Progress) << "59 %";

	Log(LogLevel::Info) << "-> Distributing Factories";
	Log(LogLevel::Progress) << "60 %";

	Log(LogLevel::Info) << "-> Distributing Pops";
	Log(LogLevel::Progress) << "61 %";

	Log(LogLevel::Info) << "-> Releasing Invasive Fauna Into Colonies";
	Log(LogLevel::Progress) << "62 %";

	Log(LogLevel::Info) << "-> Monitoring Native Fauna Reaction";
	Log(LogLevel::Progress) << "63 %";

	Log(LogLevel::Info) << "-> Dropping Infected AI Cores";
	Log(LogLevel::Progress) << "64 %";

	Log(LogLevel::Info) << "-> Dropping Poorly-Shaped States";
	Log(LogLevel::Progress) << "65 %";

	Log(LogLevel::Info) << "-> Merging Nations";
	Log(LogLevel::Info) << "-> Invoking the Undead";
	Log(LogLevel::Progress) << "66 %";

	Log(LogLevel::Info) << "-> Converting Armies and Navies";
	Log(LogLevel::Progress) << "67 %";

	Log(LogLevel::Info) << "-> Converting Ongoing Conflicts";
	Log(LogLevel::Progress) << "68 %";

	Log(LogLevel::Info) << "-> Describing Religion";
	Log(LogLevel::Progress) << "69 %";

	Log(LogLevel::Info) << "-> Converting Botanical Definitions";
	Log(LogLevel::Progress) << "70 %";

	Log(LogLevel::Info) << "-> Converting country flags";
	Log(LogLevel::Progress) << "71 %";

	Log(LogLevel::Info) << "---> Le Dump <---";
	output();

	Log(LogLevel::Info) << "*** Goodbye, Vicky 3, and godspeed. ***";
}

void V3::World::output() const
{
	commonItems::TryCreateFolder("output");

	// Delete broken remnants if any
	if (commonItems::DoesFolderExist("output/output"))
		commonItems::DeleteFolder("output/output");

	// Delete old conversion
	if (commonItems::DoesFolderExist("output/" + outputName))
	{
		Log(LogLevel::Info) << "<< Deleting existing mod folder.";
		commonItems::DeleteFolder("output/" + outputName);
	}
	Log(LogLevel::Progress) << "80 %";

	Log(LogLevel::Info) << "<- Copying Mod Template";
	commonItems::CopyFolder("blankMod/output", "output/output");
	Log(LogLevel::Progress) << "81 %";

	Log(LogLevel::Info) << "<- Moving Mod Template >> " << outputName;
	commonItems::RenameFolder("output/output", "output/" + outputName);
	Log(LogLevel::Progress) << "82 %";

	Log(LogLevel::Info) << "<- Crafting .mod File";
	createModFile();
	Log(LogLevel::Progress) << "83 %";

	// Record converter version
	Log(LogLevel::Info) << "<- Writing version";
	outputVersion();
	Log(LogLevel::Progress) << "84 %";

	// Update bookmark starting dates
	Log(LogLevel::Info) << "<- Updating bookmarks";
	Log(LogLevel::Progress) << "85 %";

	// Create common\countries path.
	Log(LogLevel::Progress) << "86 %";

	// Output common\countries.txt
	Log(LogLevel::Info) << "<- Creating countries.txt";
	Log(LogLevel::Progress) << "87 %";

	Log(LogLevel::Info) << "-> Creating Flags";
	Log(LogLevel::Progress) << "88 %";

	Log(LogLevel::Info) << "-> Setting Flags";
	Log(LogLevel::Progress) << "89 %";

	Log(LogLevel::Info) << "<- Writing Flags";
	Log(LogLevel::Progress) << "90 %";

	Log(LogLevel::Info) << "<- Writing Localization Text";
	Log(LogLevel::Progress) << "91 %";

	Log(LogLevel::Info) << "<- Writing Provinces";
	Log(LogLevel::Progress) << "92 %";

	Log(LogLevel::Info) << "<- Writing Countries";
	Log(LogLevel::Progress) << "93 %";

	Log(LogLevel::Info) << "<- Writing Diplomacy";
	Log(LogLevel::Progress) << "94 %";

	Log(LogLevel::Info) << "<- Writing Armed and Unarmed Conflicts";
	Log(LogLevel::Progress) << "95 %";

	Log(LogLevel::Info) << "<- Writing Culture Definitions";
	Log(LogLevel::Progress) << "96 %";

	Log(LogLevel::Info) << "<- Writing Religion Definitions";
	Log(LogLevel::Info) << "<- Writing Pops";
	Log(LogLevel::Progress) << "97 %";

	Log(LogLevel::Info) << "<- Sending Botanical Expedition";
	Log(LogLevel::Progress) << "98 %";

	Log(LogLevel::Info) << "<- Writing Treatise on the Origins of Invasive Fauna";
	Log(LogLevel::Progress) << "99 %";

	// verify countries got written
	Log(LogLevel::Info) << "-> Verifying All Countries Written";
}

void V3::World::outputVersion() const
{
	std::ofstream output("output/" + outputName + "/eu4tov3_version.txt");
	if (!output.is_open())
		throw std::runtime_error("Error writing version file! Is the output folder writable?");
	output << converterVersion;
	output.close();
}

void V3::World::createModFile() const
{
	std::ofstream output("output/" + outputName + "/.metadata/metadata.json");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + "/.metadata/metadata.json");
	Log(LogLevel::Info) << "<< Writing to: "
							  << "output/" + outputName + "/.metadata/metadata.json";
	outModFile(output, outputName);
	output.close();
}