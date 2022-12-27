#include "outWorld.h"
#include "CommonFunctions.h"
#include "OSCompatibilityLayer.h"
#include "outCountries/outCountries.h"
#include "outCultures/outCultures.h"
#include "outLocalizations/outLocalizations.h"
#include "outMetadataFile/outMetadataFile.h"
#include "outPops/outPops.h"
#include "outReligions/outReligions.h"
#include "outStates/outCommonHistoryStates.h"
#include <fstream>

void OUT::exportWorld(const Configuration& configuration, const V3::World& world, const commonItems::ConverterVersion& converterVersion)
{
	const auto& outputName = configuration.getOutputName();

	Log(LogLevel::Info) << "---> Le Dump <---";

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

	Log(LogLevel::Info) << "<- Crafting .metadata File";
	exportMetadataFile(outputName);
	Log(LogLevel::Progress) << "83 %";

	// Record converter version
	Log(LogLevel::Info) << "<- Writing version";
	exportVersion(outputName, converterVersion);
	Log(LogLevel::Progress) << "84 %";

	// Update bookmark starting dates
	Log(LogLevel::Info) << "<- Updating bookmarks";
	exportBookmark(outputName, configuration, world.getDatingData());

	Log(LogLevel::Info) << "<- Dumping common/history/states";
	exportCommonHistoryStates(outputName, world.getClayManager().getStates());
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

	Log(LogLevel::Info) << "<- Writing Localizations";
	exportCountryNamesAndAdjectives(outputName, world.getPoliticalManager().getCountries());
	exportReligionLocs(outputName, world.getReligionMapper().getV3ReligionDefinitions());
	exportCultureLocs(outputName, world.getCultureMapper().getV3CultureDefinitions());
	Log(LogLevel::Progress) << "91 %";

	Log(LogLevel::Info) << "<- Writing Provinces";
	Log(LogLevel::Progress) << "92 %";

	Log(LogLevel::Info) << "<- Writing Countries";
	exportCommonCountries(outputName, world.getPoliticalManager().getCountries());
	exportHistoryCountries(outputName, world.getPoliticalManager().getCountries());
	exportHistoryPopulations(outputName, world.getPoliticalManager().getCountries());
	Log(LogLevel::Progress) << "93 %";

	Log(LogLevel::Info) << "<- Writing Diplomacy";
	Log(LogLevel::Progress) << "94 %";

	Log(LogLevel::Info) << "<- Writing Armed and Unarmed Conflicts";
	Log(LogLevel::Progress) << "95 %";

	Log(LogLevel::Info) << "<- Writing Culture Definitions";
	exportCultures(outputName, world.getCultureMapper().getV3CultureDefinitions());
	Log(LogLevel::Progress) << "96 %";

	Log(LogLevel::Info) << "<- Writing Religion Definitions";
	exportReligions(outputName, world.getReligionMapper().getV3ReligionDefinitions());

	Log(LogLevel::Info) << "<- Writing Pops";
	exportPops(outputName, world.getClayManager().getStates());
	Log(LogLevel::Progress) << "97 %";

	Log(LogLevel::Info) << "<- Sending Botanical Expedition";
	Log(LogLevel::Progress) << "98 %";

	Log(LogLevel::Info) << "<- Writing Treatise on the Origins of Invasive Fauna";
	Log(LogLevel::Progress) << "99 %";

	// verify countries got written
	Log(LogLevel::Info) << "-> Verifying All Countries Written";
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
