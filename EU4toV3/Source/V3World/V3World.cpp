#include "V3World.h"
#include "../EU4World/World.h"
#include "../Helpers/TechValues.h"
#include "../Mappers/TechGroups/TechGroupsMapper.h"
#include "../Mappers/VersionParser/VersionParser.h"
#include "Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include <filesystem>
#include <fstream>
namespace fs = std::filesystem;


V2::World::World(const std::shared_ptr<Configuration>& theConfiguration,
	 const EU4::World& sourceWorld,
	 const mappers::IdeaEffectMapper& ideaEffectMapper,
	 const mappers::TechGroupsMapper& techGroupsMapper,
	 const mappers::VersionParser& versionParser):
	 historicalData(sourceWorld.getHistoricalData())
{
	Log(LogLevel::Progress) << "45 %";

	LOG(LogLevel::Info) << "*** Hello Vicky 2, creating world. ***";
	LOG(LogLevel::Info) << "-> Importing Provinces";
	Log(LogLevel::Progress) << "46 %";

	LOG(LogLevel::Info) << "-> Importing Vanilla Pops";
	Log(LogLevel::Progress) << "47 %";

	LOG(LogLevel::Info) << "-> Importing Potential Countries";
	Log(LogLevel::Progress) << "48 %";

	LOG(LogLevel::Info) << "-> Loading Country Mapping Rules";
	Log(LogLevel::Progress) << "49 %";

	LOG(LogLevel::Info) << "-> Loading Culture Mapping Rules";
	Log(LogLevel::Progress) << "50 %";

	LOG(LogLevel::Info) << "-> Pouring From Hollow Into Empty";
	Log(LogLevel::Progress) << "51 %";

	LOG(LogLevel::Info) << "-> Converting Countries";
	Log(LogLevel::Progress) << "52 %";

	LOG(LogLevel::Info) << "-> Converting Provinces";
	Log(LogLevel::Progress) << "53 %";

	LOG(LogLevel::Info) << "-> Cataloguing Invasive Fauna";
	Log(LogLevel::Progress) << "54 %";

	LOG(LogLevel::Info) << "-> Converting Diplomacy";
	Log(LogLevel::Progress) << "55 %";

	LOG(LogLevel::Info) << "-> Setting Up States";
	Log(LogLevel::Progress) << "57 %";

	LOG(LogLevel::Info) << "-> Generating Unciv Reforms";
	Log(LogLevel::Progress) << "58 %";

	LOG(LogLevel::Info) << "-> Converting Technology Levels";
	Log(LogLevel::Progress) << "59 %";

	LOG(LogLevel::Info) << "-> Distributing Factories";
	Log(LogLevel::Progress) << "60 %";

	LOG(LogLevel::Info) << "-> Distributing Pops";
	Log(LogLevel::Progress) << "61 %";

	LOG(LogLevel::Info) << "-> Releasing Invasive Fauna Into Colonies";
	Log(LogLevel::Progress) << "62 %";

	LOG(LogLevel::Info) << "-> Monitoring Native Fauna Reaction";
	Log(LogLevel::Progress) << "63 %";

	Log(LogLevel::Info) << "-> Dropping Infected AI Cores";
	Log(LogLevel::Progress) << "64 %";

	Log(LogLevel::Info) << "-> Dropping Poorly-Shaped States";
	Log(LogLevel::Progress) << "65 %";

	LOG(LogLevel::Info) << "-> Merging Nations";
	Log(LogLevel::Info) << "-> Invoking the Undead";
	Log(LogLevel::Progress) << "66 %";

	LOG(LogLevel::Info) << "-> Converting Armies and Navies";
	Log(LogLevel::Progress) << "67 %";

	LOG(LogLevel::Info) << "-> Converting Ongoing Conflicts";
	Log(LogLevel::Progress) << "68 %";

	LOG(LogLevel::Info) << "-> Describing Religion";
	Log(LogLevel::Progress) << "69 %";

	LOG(LogLevel::Info) << "-> Converting Botanical Definitions";
	transcribeHistoricalData();
	Log(LogLevel::Progress) << "70 %";

	LOG(LogLevel::Info) << "-> Converting country flags";
	Log(LogLevel::Progress) << "71 %";

	LOG(LogLevel::Info) << "---> Le Dump <---";
	output(versionParser, theConfiguration->getOutputName());

	LOG(LogLevel::Info) << "*** Goodbye, Vicky 2, and godspeed. ***";
}

void V2::World::output(const mappers::VersionParser& versionParser, const std::string& outputName) const
{
	commonItems::TryCreateFolder("output");
	Log(LogLevel::Progress) << "80 %";

	LOG(LogLevel::Info) << "<- Copying Mod Template";
	commonItems::CopyFolder("blankMod/output", "output/output");
	Log(LogLevel::Progress) << "81 %";

	LOG(LogLevel::Info) << "<- Moving Mod Template >> " << outputName;
	commonItems::RenameFolder("output/output", "output/" + outputName);
	Log(LogLevel::Progress) << "82 %";

	LOG(LogLevel::Info) << "<- Crafting .mod File";
	createModFile(outputName);
	Log(LogLevel::Progress) << "83 %";

	// Record converter version
	LOG(LogLevel::Info) << "<- Writing version";
	outputVersion(versionParser, outputName);
	Log(LogLevel::Progress) << "84 %";

	// Update bookmark starting dates
	LOG(LogLevel::Info) << "<- Updating bookmarks";
	Log(LogLevel::Progress) << "85 %";

	// Create common\countries path.
	const auto countriesPath = "output/" + outputName + "/common/countries";
	if (!commonItems::TryCreateFolder(countriesPath))
	{
		return;
	}
	Log(LogLevel::Progress) << "86 %";

	// Output common\countries.txt
	LOG(LogLevel::Info) << "<- Creating countries.txt";
	Log(LogLevel::Progress) << "87 %";

	// Create flags for all new countries.
	LOG(LogLevel::Info) << "-> Creating Flags";
	Log(LogLevel::Progress) << "88 %";

	LOG(LogLevel::Info) << "-> Setting Flags";
	Log(LogLevel::Progress) << "89 %";

	LOG(LogLevel::Info) << "<- Writing Flags";
	Log(LogLevel::Progress) << "90 %";

	// Create localizations for all new countries. We don't actually know the names yet so we just use the tags as the names.
	LOG(LogLevel::Info) << "<- Writing Localisation Text";
	Log(LogLevel::Progress) << "91 %";

	LOG(LogLevel::Info) << "<- Writing Provinces";
	Log(LogLevel::Progress) << "92 %";

	LOG(LogLevel::Info) << "<- Writing Countries";
	Log(LogLevel::Progress) << "93 %";

	LOG(LogLevel::Info) << "<- Writing Diplomacy";
	Log(LogLevel::Progress) << "94 %";

	LOG(LogLevel::Info) << "<- Writing Armed and Unarmed Conflicts";
	Log(LogLevel::Progress) << "95 %";

	LOG(LogLevel::Info) << "<- Writing Culture Definitions";
	Log(LogLevel::Progress) << "96 %";

	LOG(LogLevel::Info) << "<- Writing Religion Definitions";
	LOG(LogLevel::Info) << "<- Writing Pops";
	Log(LogLevel::Progress) << "97 %";

	LOG(LogLevel::Info) << "<- Sending Botanical Expedition";
	outputHistory(outputName);
	Log(LogLevel::Progress) << "98 %";

	LOG(LogLevel::Info) << "<- Writing Treatise on the Origins of Invasive Fauna";
	Log(LogLevel::Progress) << "99 %";

	// verify countries got written
	LOG(LogLevel::Info) << "-> Verifying All Countries Written";
}

void V2::World::transcribeHistoricalData()
{
	std::vector<std::pair<std::string, EU4::HistoricalEntry>> transcribedData;
	for (const auto& entry: historicalData)
	{
		const auto& possibleTag = countryMapper.getV2Tag(entry.first);
		if (possibleTag)
			transcribedData.emplace_back(std::make_pair(*possibleTag, entry.second));
	}
	historicalData.swap(transcribedData);
}

void V2::World::outputHistory(const std::string& outputName) const
{
	std::ofstream output("output/" + outputName + "/common/botanical_expedition.txt");
	if (!output.is_open())
	{
		throw std::runtime_error(
			 "Could not send botanical expedition output/" + outputName + "/common/botanical_expedition.txt - " + commonItems::GetLastErrorString());
	}
	output << historicalData;
	output.close();
}

void V2::World::outputVersion(const mappers::VersionParser& versionParser, const std::string& outputName)
{
	std::ofstream output("output/" + outputName + "/eu4tov3_version.txt");
	if (!output.is_open())
		throw std::runtime_error("Error writing version file! Is the output folder writable?");
	output << versionParser;
	output.close();
}


void V2::World::createModFile(const std::string& outputName) const
{
	std::ofstream output("output/" + outputName + ".mod");
	if (!output.is_open())
		throw std::runtime_error("Could not create " + outputName + ".mod");
	LOG(LogLevel::Info) << "\t-> Writing to: "
							  << "output/" + outputName + ".mod";
	output << modFile;
	output.close();
}
