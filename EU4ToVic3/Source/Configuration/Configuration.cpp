#include "Configuration.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "GameVersion.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <fstream>

Configuration::Configuration(const commonItems::ConverterVersion& converterVersion)
{
	Log(LogLevel::Info) << "Reading configuration file";
	registerKeys();
	parseFile("configuration.txt");
	clearRegisteredKeywords();
	setOutputName();
	verifyEU4Path();
	verifyEU4Version(converterVersion);
	verifyVic3Path();
	verifyVic3Version(converterVersion);
	Log(LogLevel::Progress) << "3 %";
}

Configuration::Configuration(std::istream& theStream, const commonItems::ConverterVersion& converterVersion)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	setOutputName();
	verifyEU4Path();
	verifyEU4Version(converterVersion);
	verifyVic3Path();
	verifyVic3Version(converterVersion);
}

void Configuration::registerKeys()
{
	// ------ config stuff

	registerKeyword("SaveGame", [this](std::istream& theStream) {
		EU4SaveGamePath = commonItems::getString(theStream);
		Log(LogLevel::Info) << "EU4 savegame path: " << EU4SaveGamePath;
	});
	registerKeyword("EU4directory", [this](std::istream& theStream) {
		EU4Path = commonItems::getString(theStream);
		Log(LogLevel::Info) << "EU4 path: " << EU4Path;
	});
	registerKeyword("EU4DocumentsDirectory", [this](std::istream& theStream) {
		EU4DocumentsPath = commonItems::getString(theStream);
		Log(LogLevel::Info) << "EU4 documents path: " << EU4DocumentsPath;
	});
	registerKeyword("Vic3directory", [this](std::istream& theStream) {
		Vic3Path = commonItems::getString(theStream);
		Log(LogLevel::Info) << "Vic3 path: " << Vic3Path;
	});

	// ------- options

	registerKeyword("start_date", [this](std::istream& theStream) {
		const auto startDateString = commonItems::getString(theStream);
		configBlock.startDate = static_cast<STARTDATE>(std::stoi(startDateString));
		Log(LogLevel::Info) << "Start Date: " << startDateString;
	});
	registerKeyword("remove_type", [this](std::istream& theStream) {
		const auto removeTypeString = commonItems::getString(theStream);
		configBlock.removeType = static_cast<DEADCORES>(std::stoi(removeTypeString));
		Log(LogLevel::Info) << "Releasables Removal: " << removeTypeString;
	});
	registerKeyword("pop_shaping", [this](std::istream& theStream) {
		const auto popShapingString = commonItems::getString(theStream);
		configBlock.popShaping = static_cast<POPSHAPES>(std::stoi(popShapingString));
		Log(LogLevel::Info) << "Pop Shaping: " << popShapingString;
	});
	registerKeyword("euro_centrism", [this](std::istream& theStream) {
		const auto euroCentrismString = commonItems::getString(theStream);
		configBlock.euroCentric = static_cast<EUROCENTRISM>(std::stoi(euroCentrismString));
		Log(LogLevel::Info) << "Eurocentrism: " << euroCentrismString;
	});
	registerKeyword("convert_all", [this](std::istream& theStream) {
		const auto convertAllString = commonItems::getString(theStream);
		configBlock.convertAll = convertAllString == "yes";
		Log(LogLevel::Info) << "Convert All: " << convertAllString;
	});
	registerKeyword("economy", [this](std::istream& theStream) {
		const auto economyString = commonItems::getString(theStream);
		configBlock.economy = static_cast<ECONOMY>(std::stoi(economyString));
		Log(LogLevel::Info) << "Economy: " << economyString;
	});
	registerKeyword("downtiers", [this](std::istream& theStream) {
		const auto downTiersString = commonItems::getString(theStream);
		configBlock.downTiers = downTiersString == "yes";
		Log(LogLevel::Info) << "Downgrade Tiers: " << downTiersString;
	});
	registerKeyword("splittcs", [this](std::istream& theStream) {
		const auto splitTCsString = commonItems::getString(theStream);
		configBlock.splitTCs = static_cast<SPLITTCS>(std::stoi(splitTCsString));
		Log(LogLevel::Info) << "Split TCs: " << splitTCsString;
	});
	registerKeyword("output_name", [this](std::istream& theStream) {
		outputName = commonItems::getString(theStream);
		Log(LogLevel::Info) << "Output Name: " << outputName;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void Configuration::verifyEU4Path() const
{
	if (!commonItems::DoesFolderExist(EU4Path))
		throw std::runtime_error("EU4 path " + EU4Path + " does not exist!");
	if (!commonItems::DoesFileExist(EU4Path + "/eu4.exe") && !commonItems::DoesFileExist(EU4Path + "/eu4") &&
		 !commonItems::DoesFolderExist(EU4Path + "/eu4.app"))
		throw std::runtime_error(EU4Path + " does not contain Europa Universalis 4!");
	if (!commonItems::DoesFileExist(EU4Path + "/map/positions.txt"))
		throw std::runtime_error(EU4Path + " does not appear to be a valid EU4 install!");
	Log(LogLevel::Info) << "\tEU4 install path is " << EU4Path;
}

void Configuration::verifyVic3Path()
{
	if (!commonItems::DoesFolderExist(Vic3Path))
		throw std::runtime_error("Vic3 path " + Vic3Path + " does not exist!");
	// TODO: OSX and Linux paths are speculative
	// TODO: As a matter of fact...
	if (!commonItems::DoesFileExist(Vic3Path + "/binaries/victoria3.exe") && !commonItems::DoesFileExist(Vic3Path + "/Vic3game") &&
		 !commonItems::DoesFileExist(Vic3Path + "/binaries/victoria3"))
		throw std::runtime_error(Vic3Path + " does not contain Victoria 3!");
	if (!commonItems::DoesFileExist(Vic3Path + "/game/map_data/provinces.png"))
		throw std::runtime_error(Vic3Path + " does not appear to be a valid Vic3 install!");
	Log(LogLevel::Info) << "\tVic3 install path is " << Vic3Path;
	Vic3Path += "/game/"; // We're adding "/game/" since all we ever need from now on is in that subdirectory.
}

void Configuration::setOutputName()
{
	if (outputName.empty())
	{
		outputName = trimPath(EU4SaveGamePath);
	}

	outputName = trimExtension(outputName);
	outputName = replaceCharacter(outputName, '-');
	outputName = replaceCharacter(outputName, ' ');

	outputName = commonItems::normalizeUTF8Path(outputName);
	Log(LogLevel::Info) << "Using output name " << outputName;
}

void Configuration::verifyEU4Version(const commonItems::ConverterVersion& converterVersion) const
{
	const auto EU4Version = GameVersion::extractVersionFromLauncher(EU4Path + "/launcher-settings.json");
	if (!EU4Version)
	{
		Log(LogLevel::Error) << "EU4 version could not be determined, proceeding blind!";
		return;
	}

	Log(LogLevel::Info) << "EU4 version: " << EU4Version->toShortString();

	if (converterVersion.getMinSource() > *EU4Version)
	{
		Log(LogLevel::Error) << "EU4 version is v" << EU4Version->toShortString() << ", converter requires minimum v"
									<< converterVersion.getMinSource().toShortString() << "!";
		throw std::runtime_error("Converter vs EU4 installation mismatch!");
	}
	if (!converterVersion.getMaxSource().isLargerishThan(*EU4Version))
	{
		Log(LogLevel::Error) << "EU4 version is v" << EU4Version->toShortString() << ", converter requires maximum v"
									<< converterVersion.getMaxSource().toShortString() << "!";
		throw std::runtime_error("Converter vs EU4 installation mismatch!");
	}
}

void Configuration::verifyVic3Version(const commonItems::ConverterVersion& converterVersion) const
{
	const auto V3Version = GameVersion::extractVersionFromLauncher(Vic3Path + "../launcher/launcher-settings.json");
	if (!V3Version)
	{
		Log(LogLevel::Error) << "Vic3 version could not be determined, proceeding blind!";
		return;
	}

	Log(LogLevel::Info) << "Vic3 version: " << V3Version->toShortString();

	if (converterVersion.getMinTarget() > *V3Version)
	{
		Log(LogLevel::Error) << "Vic3 version is v" << V3Version->toShortString() << ", converter requires minimum v"
									<< converterVersion.getMinTarget().toShortString() << "!";
		throw std::runtime_error("Converter vs Vic3 installation mismatch!");
	}
	if (!converterVersion.getMaxTarget().isLargerishThan(*V3Version))
	{
		Log(LogLevel::Error) << "Vic3 version is v" << V3Version->toShortString() << ", converter requires maximum v"
									<< converterVersion.getMaxTarget().toShortString() << "!";
		throw std::runtime_error("Converter vs Vic3 installation mismatch!");
	}
}
