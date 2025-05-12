#include "outWorld.h"
#include "CommonFunctions.h"
#include "OSCompatibilityLayer.h"
#include "outBuildings/outBuildings.h"
#include "outCharacters/outCharacters.h"
#include "outCoAs/outCoAa.h"
#include "outCountries/outCountries.h"
#include "outCultures/outCultures.h"
#include "outDiplomacy/outDiplomacy.h"
#include "outDiplomaticPlays/outDiplomaticPlays.h"
#include "outFlagDefinitions/outFlagDefinitions.h"
#include "outLocalizations/outLocalizations.h"
#include "outMetadataFile/outMetadataFile.h"
#include "outPops/outPops.h"
#include "outReligions/outReligions.h"
#include "outSecretGoals/outSecretGoals.h"
#include "outStates/outCommonHistoryStates.h"
#include "outStrategies/outStrategies.h"
#include <fstream>

void OUT::exportWorld(const Configuration& configuration, const V3::World& world, const commonItems::ConverterVersion& converterVersion)
{
	const auto& outputName = configuration.getOutputName();
	const auto& knownLocs = world.getVanillaLocalizations();

	Log(LogLevel::Info) << "---> Le Dump <---";
	commonItems::TryCreateFolder("output");
	Log(LogLevel::Progress) << "80 %";

	// Delete old conversion
	if (commonItems::DoesFolderExist("output/" + outputName))
	{
		Log(LogLevel::Info) << "<< Deleting existing mod folder.";
		if (!commonItems::DeleteFolder("output/" + outputName))
			throw std::runtime_error("Could not delete existing output/" + outputName + "! Please delete it manually and retry.");
	}
	Log(LogLevel::Progress) << "81 %";

	Log(LogLevel::Info) << "<< Copying Mod Template from blankMod/output to output/" << outputName;
	if (!commonItems::CopyFolder("blankMod/output", "output/" + outputName))
		throw std::runtime_error("Error copying mod template! Is the output/ folder writable?");

	if (configuration.configBlock.thirdOdyssey)
	{
		// Disabling TO custom mapmodding until vic3 map is stable.
		// if (!commonItems::TryCopyFile("configurables/third_odyssey/map_data/state_regions/05_north_america.txt",
		//		  "output/" + outputName + "/map_data/state_regions/05_north_america.txt"))
		//	throw std::runtime_error("Error copying TO map_data! Is the output/ folder writable?");
		if (!commonItems::CopyFolder("configurables/third_odyssey/localization", "output/" + outputName + "/localization"))
			throw std::runtime_error("Error copying TO locs! Is the output/ folder writable?");
		// if (!commonItems::CopyFolder("configurables/third_odyssey/gfx", "output/" + outputName + "/gfx"))
		//	throw std::runtime_error("Error copying TO gfx! Is the output/ folder writable?");
	}

	Log(LogLevel::Progress) << "83 %";

	Log(LogLevel::Info) << "<- Crafting .metadata File";
	exportMetadataFile(outputName);
	Log(LogLevel::Progress) << "84 %";

	// Record converter version
	Log(LogLevel::Info) << "<- Writing version";
	exportVersion(outputName, converterVersion);
	Log(LogLevel::Progress) << "85 %";

	// Update bookmark starting dates
	Log(LogLevel::Info) << "<- Updating bookmarks";
	exportBookmark(outputName, configuration, world.getDatingData());
	Log(LogLevel::Progress) << "86 %";

	Log(LogLevel::Info) << "<- Dumping common/history/states";
	exportCommonHistoryStates(outputName, world.getClayManager().getStates());
	Log(LogLevel::Progress) << "87 %";

	Log(LogLevel::Info) << "<- Copying Custom Flags";
	copyCustomFlags(outputName);
	Log(LogLevel::Progress) << "88 %";
	Log(LogLevel::Info) << "<- Writing Custom CoAs";
	exportCustomCoAs(outputName, world.getPoliticalManager().getCountries());
	Log(LogLevel::Progress) << "89 %";
	Log(LogLevel::Info) << "<- Writing Flag Definitions";
	exportFlagDefinitions(outputName, world.getPoliticalManager().getCountries());
	Log(LogLevel::Progress) << "90 %";

	Log(LogLevel::Info) << "<- Writing Localizations";
	exportCountryNamesAndAdjectives(outputName, world.getPoliticalManager().getCountries(), knownLocs);
	exportReligionLocs(outputName, world.getReligionMapper().getV3ReligionDefinitions(), knownLocs);
	exportCultureLocs(outputName, world.getCultureMapper().getV3CultureDefinitions(), knownLocs);
	exportCharacterLocs(outputName, world.getPoliticalManager().getCountries(), knownLocs);
	exportPowerBlocLocs(outputName, world.getPoliticalManager().getPowerBlocs());
	Log(LogLevel::Progress) << "91 %";

	Log(LogLevel::Info) << "<- Writing Countries";
	exportCommonCountries(outputName, world.getPoliticalManager().getCountries());
	exportHistoryCountries(outputName, world.getPoliticalManager().getCountries());
	exportHistoryPopulations(outputName, world.getPoliticalManager().getCountries());
	exportReleasables(outputName, world.getPoliticalManager().getCountries());
	exportMajorFormables(outputName, world.getPoliticalManager().getMajorFormables());
	Log(LogLevel::Progress) << "92 %";

	Log(LogLevel::Info) << "<- Writing Diplomacy";
	exportDiplomacy(outputName, world.getPoliticalManager());
	exportDiplomaticPlays(outputName, world.getPoliticalManager().getCountries());
	Log(LogLevel::Progress) << "93 %";

	// Log(LogLevel::Info) << "<- Writing Armed and Unarmed Conflicts";

	Log(LogLevel::Info) << "<- Writing AI Strategies";
	exportStrategies(outputName, world.getPoliticalManager().getCountries());
	exportSecretGoals(outputName, world.getPoliticalManager().getCountries());
	Log(LogLevel::Progress) << "94 %";

	Log(LogLevel::Info) << "<- Writing Culture Definitions";
	exportCultures(outputName, world.getCultureMapper().getV3CultureDefinitions());
	Log(LogLevel::Progress) << "95 %";

	Log(LogLevel::Info) << "<- Writing Religion Definitions";
	exportReligions(outputName, world.getReligionMapper().getV3ReligionDefinitions());
	Log(LogLevel::Progress) << "96 %";

	Log(LogLevel::Info) << "<- Writing Pops";
	exportCharacters(outputName, world.getPoliticalManager().getCountries());
	Log(LogLevel::Progress) << "97 %";

	Log(LogLevel::Info) << "<- Writing Characters";
	exportPops(outputName, world.getClayManager().getStates());
	Log(LogLevel::Progress) << "98 %";

	Log(LogLevel::Info) << "<- Writing Buildings";
	exportBuildings(outputName, world.getClayManager().getStates());
	Log(LogLevel::Progress) << "99 %";
}

void OUT::exportVersion(const std::string& outputName, const commonItems::ConverterVersion& converterVersion)
{
	std::ofstream output("output/" + outputName + "/eu4tovic3_version.txt");
	if (!output.is_open())
		throw std::runtime_error("Error writing version file! Is the output folder writable?");
	output << converterVersion;
	output.close();
}

void OUT::exportBookmark(const std::string& outputName, const Configuration& configuration, const DatingData& datingData)
{
	if (configuration.configBlock.startDate == Configuration::STARTDATE::Vanilla)
		return;
	std::ofstream output("output/" + outputName + "/common/defines/99_converter_defines.txt");
	if (!output.is_open())
		throw std::runtime_error("Error writing defines file! Is the output folder writable?");
	output << commonItems::utf8BOM << "NGame = { START_DATE = \"" << datingData.lastEU4Date << "\" }\n";
	output.close();
}

void OUT::copyCustomFlags(const std::string& outputName)
{
	auto counter = 0;
	if (!commonItems::DoesFolderExist("flags.tmp"))
	{
		Log(LogLevel::Warning) << "Flag folder flags.tmp not found!";
		return;
	}
	for (const auto& filename: commonItems::GetAllFilesInFolder("flags.tmp"))
	{
		if (commonItems::TryCopyFile("flags.tmp/" + filename, "output/" + outputName + "/gfx/coat_of_arms/textured_emblems/custom_export/" + filename))
			++counter;
	}
	Log(LogLevel::Info) << "<< " << counter << " flags copied.";
}
