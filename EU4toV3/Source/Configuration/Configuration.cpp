#include "Configuration.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <fstream>

Configuration::Configuration()
{
	Log(LogLevel::Info) << "Reading configuration file";
	registerKeys();
	parseFile("configuration.txt");
	clearRegisteredKeywords();
	setOutputName();
	verifyEU4Path();
	verifyVic3Path();
	Log(LogLevel::Progress) << "3 %";
}

Configuration::Configuration(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
	setOutputName();
	verifyEU4Path();
	verifyVic3Path();
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
		Log(LogLevel::Info) << "Vic2 path: " << Vic3Path;
	});

	// ------- options

	registerKeyword("max_literacy", [this](std::istream& theStream) {
		const auto maxLiteracyString = commonItems::getString(theStream);
		configBlock.MaxLiteracy = static_cast<double>(std::stoi(maxLiteracyString)) / 100;
		Log(LogLevel::Info) << "Max Literacy: " << configBlock.MaxLiteracy;
	});
	registerKeyword("remove_type", [this](std::istream& theStream) {
		const auto removeTypeString = commonItems::getString(theStream);
		configBlock.removeType = static_cast<DEADCORES>(std::stoi(removeTypeString));
		Log(LogLevel::Info) << "Core Removal: " << removeTypeString;
	});
	registerKeyword("pop_shaping", [this](std::istream& theStream) {
		const auto popShapingString = commonItems::getString(theStream);
		configBlock.popShaping = static_cast<POPSHAPES>(std::stoi(popShapingString));
		Log(LogLevel::Info) << "Pop Shaping: " << popShapingString;
	});
	registerKeyword("core_handling", [this](std::istream& theStream) {
		const auto coreHandlingString = commonItems::getString(theStream);
		configBlock.coreHandling = static_cast<COREHANDLES>(std::stoi(coreHandlingString));
		Log(LogLevel::Info) << "Core Handling: " << coreHandlingString;
	});
	registerKeyword("pop_shaping_factor", [this](std::istream& theStream) {
		const auto popShapingFactorString = commonItems::getString(theStream);
		configBlock.popShapingFactor = static_cast<double>(std::stoi(popShapingFactorString));
		Log(LogLevel::Info) << "Pop Shaping Factor: " << configBlock.popShapingFactor;
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
	registerKeyword("output_name", [this](std::istream& theStream) {
		outputName = commonItems::getString(theStream);
		Log(LogLevel::Info) << "Output Name: " << outputName;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

void Configuration::verifyEU4Path() const
{
	if (!commonItems::DoesFolderExist(EU4Path))
		throw std::runtime_error(EU4Path + " does not exist!");
	if (!commonItems::DoesFileExist(EU4Path + "/eu4.exe") && !commonItems::DoesFileExist(EU4Path + "/eu4") &&
		 !commonItems::DoesFolderExist(EU4Path + "/eu4.app"))
		throw std::runtime_error(EU4Path + " does not contain Europa Universalis 4!");
	if (!commonItems::DoesFileExist(EU4Path + "/map/positions.txt"))
		throw std::runtime_error(EU4Path + " does not appear to be a valid EU4 install!");
}

void Configuration::verifyVic3Path()
{
	if (!commonItems::DoesFolderExist(Vic3Path))
		throw std::runtime_error(Vic3Path + " does not exist!");
	// TODO: OSX and Linux paths are speculative
	// TODO: As a matter of fact...
	if (!commonItems::DoesFileExist(Vic3Path + "/victoria2.exe") && !commonItems::DoesFileExist(Vic3Path + "/Vic3game") &&
		 !commonItems::DoesFileExist(Vic3Path + "/binaries/victoria3"))
		throw std::runtime_error(Vic3Path + " does not contain Victoria 3!");
	if (!commonItems::DoesFileExist(Vic3Path + "/map/positions.txt"))
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
