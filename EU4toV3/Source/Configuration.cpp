#include "Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <fstream>
#include "CommonFunctions.h"
#include "CommonRegexes.h"

Configuration theConfiguration;

void Configuration::instantiate(std::istream& theStream, bool (*DoesFolderExist)(const std::string& path2), bool (*doesFileExist)(const std::string& path3))
{
	registerKeyword("SaveGame", [this](std::istream& theStream) {
		EU4SaveGamePath = commonItems::getString(theStream);
		LOG(LogLevel::Info) << "EU4 savegame path: " << EU4SaveGamePath;
	});
	registerKeyword("EU4directory", [this, DoesFolderExist, doesFileExist](std::istream& theStream) {
		EU4Path = commonItems::getString(theStream);
		verifyEU4Path(EU4Path, DoesFolderExist, doesFileExist);
		LOG(LogLevel::Info) << "EU4 path: " << EU4Path;
	});
	registerKeyword("EU4DocumentsDirectory", [this](std::istream& theStream) {
		EU4DocumentsPath = commonItems::getString(theStream);
		LOG(LogLevel::Info) << "EU4 documents path: " << EU4DocumentsPath;
	});
	registerKeyword("SteamWorkshopDirectory", [this](std::istream& theStream) {
		SteamWorkshopPath = commonItems::getString(theStream);
		LOG(LogLevel::Info) << "EU4 steam workshop path: " << SteamWorkshopPath;
	});
	registerKeyword("CK2ExportDirectory", [this](std::istream& theStream) {
		CK2ExportPath = commonItems::getString(theStream);
		LOG(LogLevel::Info) << "CK2 Export path: " << CK2ExportPath;
	});
	registerKeyword("Vic2directory", [this, DoesFolderExist, doesFileExist](std::istream& theStream) {
		Vic2Path = commonItems::getString(theStream);
		verifyVic2Path(Vic2Path, DoesFolderExist, doesFileExist);
		LOG(LogLevel::Info) << "Vic2 path: " << Vic2Path;
	});
	registerKeyword("Vic2Documentsdirectory", [this, DoesFolderExist](std::istream& theStream) {
		Vic2DocumentsPath = commonItems::getString(theStream);
		verifyVic2DocumentsPath(Vic2DocumentsPath, DoesFolderExist);
		LOG(LogLevel::Info) << "Vic2 documents path: " << Vic2DocumentsPath;
	});
	registerKeyword("max_literacy", [this](std::istream& theStream) {
		const auto maxLiteracyString = commonItems::getString(theStream);
		MaxLiteracy = static_cast<double>(std::stoi(maxLiteracyString)) / 100;
		LOG(LogLevel::Info) << "Max Literacy: " << MaxLiteracy;
	});
	registerKeyword("remove_type", [this](std::istream& theStream) {
		const auto removeTypeString = commonItems::getString(theStream);
		removeType = DEADCORES(std::stoi(removeTypeString));
	});
	registerKeyword("absorb_colonies", [this](std::istream& theStream) {
		const auto absorbString = commonItems::getString(theStream);
		absorbColonies = ABSORBCOLONIES(std::stoi(absorbString));
		LOG(LogLevel::Info) << "Absorb Colonies: " << absorbString;
	});
	registerKeyword("liberty_threshold", [this](std::istream& theStream) {
		const auto libertyThresholdString = commonItems::getString(theStream);
		libertyThreshold = LIBERTYDESIRE(std::stoi(libertyThresholdString));
		LOG(LogLevel::Info) << "Liberty Treshold: " << libertyThresholdString;
	});
	registerKeyword("pop_shaping", [this](std::istream& theStream) {
		const auto popShapingString = commonItems::getString(theStream);
		popShaping = POPSHAPES(std::stoi(popShapingString));
		LOG(LogLevel::Info) << "Pop Shaping: " << popShapingString;
	});
	registerKeyword("core_handling", [this](std::istream& theStream) {
		const auto coreHandlingString = commonItems::getString(theStream);
		coreHandling = COREHANDLES(std::stoi(coreHandlingString));
		LOG(LogLevel::Info) << "Core Handling: " << coreHandlingString;
	});
	registerKeyword("pop_shaping_factor", [this](std::istream& theStream) {
		const auto popShapingFactorString = commonItems::getString(theStream);
		popShapingFactor = static_cast<double>(std::stoi(popShapingFactorString));
		LOG(LogLevel::Info) << "Pop Shaping Factor: " << popShapingFactor;
	});
	registerKeyword("euro_centrism", [this](std::istream& theStream) {
		const auto euroCentrismString = commonItems::getString(theStream);
		euroCentric = EUROCENTRISM(std::stoi(euroCentrismString));
		LOG(LogLevel::Info) << "Eurocentrism: " << euroCentrismString;
	});
	registerKeyword("africa_reset", [this](std::istream& theStream) {
		const auto africaResetString = commonItems::getString(theStream);
		africaReset = AFRICARESET(std::stoi(africaResetString));
		LOG(LogLevel::Info) << "Africa Reset: " << africaResetString;
	});
	registerKeyword("debug", [this](std::istream& theStream) {
		const auto debugString = commonItems::getString(theStream);
		debug = (debugString == "yes");
	});
	registerKeyword("randomise_rgos", [this](std::istream& theStream) {
		const auto randomiseRgosString = commonItems::getString(theStream);
		randomiseRgos = randomiseRgosString == "yes";
		LOG(LogLevel::Info) << "Randomise RGOs: " << randomiseRgosString;
	});
	registerKeyword("convert_all", [this](std::istream& theStream) {
		const auto convertAllString = commonItems::getString(theStream);
		convertAll = convertAllString == "yes";
		LOG(LogLevel::Info) << "Convert All: " << convertAllString;
	});
	registerKeyword("hybrid_mod", [this](std::istream& theStream) {
		const auto hybridModString = commonItems::getString(theStream);
		hybridMod = HYBRIDMOD(std::stoi(hybridModString));
		LOG(LogLevel::Info) << "Hybrid mod: " << hybridModString;
	});
	registerKeyword("output_name", [this](std::istream& theStream) {
		incomingOutputName = commonItems::getString(theStream);
		LOG(LogLevel::Info) << "Output Name: " << incomingOutputName;
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	LOG(LogLevel::Info) << "Reading configuration file";
	parseStream(theStream);
	clearRegisteredKeywords();
	setOutputName();
	if (isHpmEnabled())
	{
		if (!DoesFolderExist(Vic2Path + "/mod/HPM"))
			throw std::runtime_error(Vic2Path + "/mod/HPM does not exist!");
		Vic2Path += "/mod/HPM";
	}
	Log(LogLevel::Progress) << "3 %";
}

void Configuration::verifyEU4Path(const std::string& path, bool (*DoesFolderExist)(const std::string& path2), bool (*doesFileExist)(const std::string& path3))
{
	if (!DoesFolderExist(path))
		throw std::runtime_error(path + " does not exist!");
	if (!doesFileExist(path + "/eu4.exe") && !doesFileExist(path + "/eu4") && !DoesFolderExist(path + "/eu4.app"))
		throw std::runtime_error(path + " does not contain Europa Universalis 4!");
	if (!doesFileExist(path + "/map/positions.txt"))
		throw std::runtime_error(path + " does not appear to be a valid EU4 install!");
}

void Configuration::verifyVic2Path(const std::string& path, bool (*DoesFolderExist)(const std::string& path2), bool (*doesFileExist)(const std::string& path3))
{
	if (!DoesFolderExist(path))
		throw std::runtime_error(path + " does not exist!");
	if (!doesFileExist(path + "/v2game.exe") && !DoesFolderExist(path + "/Victoria 2 - Heart Of Darkness.app") && !DoesFolderExist(path + "../../MacOS"))
		throw std::runtime_error(path + " does not contain Victoria 2!");
}

void Configuration::verifyVic2DocumentsPath(const std::string& path, bool (*DoesFolderExist)(const std::string& path2))
{
	if (!DoesFolderExist(path))
		throw std::runtime_error(path + " does not exist!");
}

void Configuration::setOutputName()
{
	if (incomingOutputName.empty())
	{
		outputName = trimPath(EU4SaveGamePath);
	}
	else
	{
		outputName = incomingOutputName;
	}
	outputName = trimExtension(outputName);
	outputName = replaceCharacter(outputName, '-');
	outputName = replaceCharacter(outputName, ' ');
	theConfiguration.setActualName(outputName);

	outputName = commonItems::normalizeUTF8Path(outputName);
	theConfiguration.setOutputName(outputName);
	LOG(LogLevel::Info) << "Using output name " << outputName;
}

ConfigurationFile::ConfigurationFile(const std::string& filename)
{
	std::ifstream confFile(filename);
	if (!confFile.is_open())
	{
		Log(LogLevel::Error) << "Cound not open configuration file!";
		throw std::runtime_error("Cound not open configuration file!");
	}

	theConfiguration.instantiate(confFile, commonItems::DoesFolderExist, commonItems::DoesFileExist);

	confFile.close();
}
