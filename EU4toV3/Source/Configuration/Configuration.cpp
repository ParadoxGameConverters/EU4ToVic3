#include "Configuration.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <fstream>

Configuration::Configuration(const mappers::ConverterVersion& converterVersion)
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

Configuration::Configuration(std::istream& theStream, const mappers::ConverterVersion& converterVersion)
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

std::optional<GameVersion> Configuration::getRawVersion(const std::string& filePath) const
{
	if (!commonItems::DoesFileExist(filePath))
	{
		Log(LogLevel::Warning) << "Failure verifying version: " << filePath << " does not exist. Proceeding blind.";
		return std::nullopt;
	}

	std::ifstream versionFile(filePath);
	if (!versionFile.is_open())
	{
		Log(LogLevel::Warning) << "Failure verifying version: " << filePath << " cannot be opened. Proceeding blind.";
		return std::nullopt;
	}

	while (!versionFile.eof())
	{
		std::string line;
		std::getline(versionFile, line);
		if (line.find("rawVersion") == std::string::npos)
			continue;
		auto pos = line.find(':');
		if (pos == std::string::npos)
		{
			Log(LogLevel::Warning) << "Failure extracting version: " << filePath << " has broken rawVersion. Proceeding blind.";
			return std::nullopt;
		}
		line = line.substr(pos + 1, line.length());
		pos = line.find_first_of('\"');
		if (pos == std::string::npos)
		{
			Log(LogLevel::Warning) << "Failure extracting version: " << filePath << " has broken rawVersion. Proceeding blind.";
			return std::nullopt;
		}
		line = line.substr(pos + 1, line.length());
		pos = line.find_first_of('\"');
		if (pos == std::string::npos)
		{
			Log(LogLevel::Warning) << "Failure extracting version: " << filePath << " has broken rawVersion. Proceeding blind.";
			return std::nullopt;
		}
		line = line.substr(0, pos);
		Log(LogLevel::Info) << "\tVersion is: " << line;
		return GameVersion(line);
	}

	Log(LogLevel::Warning) << "Failure verifying version: " << filePath << " doesn't contain rawVersion. Proceeding blind.";
	return std::nullopt;
}

void Configuration::verifyEU4Version(const mappers::ConverterVersion& converterVersion) const
{
	const auto EU4Version = getRawVersion(EU4Path + "/launcher-settings.json");
	if (!EU4Version)
	{
		Log(LogLevel::Error) << "EU4 version could not be determined, proceeding blind!";
		return;
	}

	if (converterVersion.getMinSource() > *EU4Version)
	{
		Log(LogLevel::Error) << "EU4 version is v" << EU4Version->toShortString() << ", converter requires minimum v"
									<< converterVersion.getMinSource().toShortString() << "!";
		throw std::runtime_error("Converter vs EU4 installation mismatch!");
	}
	if (!converterVersion.getMaxSource().isLargerishThan(*EU4Version))
	{
		Log(LogLevel::Error) << "EU4 version is v" << EU4Version->toShortString() << ", converter requires maximum v"
									<< converterVersion.getMinSource().toShortString() << "!";
		throw std::runtime_error("Converter vs EU4 installation mismatch!");
	}
}

void Configuration::verifyVic3Version(const mappers::ConverterVersion& converterVersion) const
{
	const auto V3Version = getRawVersion(Vic3Path + "../launcher/launcher-settings.json");
	if (!V3Version)
	{
		Log(LogLevel::Error) << "Vic3 version could not be determined, proceeding blind!";
		return;
	}

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
