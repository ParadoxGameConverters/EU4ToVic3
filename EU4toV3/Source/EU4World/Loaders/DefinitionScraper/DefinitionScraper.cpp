#include "DefinitionScraper.h"
#include "OSCompatibilityLayer.h"
#include <fstream>

void EU4::DefinitionScraper::loadDefinitions(const std::string& EU4Path, const Mods& mods)
{
	// This is a case where mods take priority over definitions. If a mod has defs than we use those and ignore EU4 installation.

	for (const auto& mod: mods)
		if (commonItems::DoesFileExist(mod.path + "/map/definition.csv"))
		{
			std::ifstream definitionsFile(mod.path + "/map/definition.csv");
			parseStream(definitionsFile);
			definitionsFile.close();
			Log(LogLevel::Info) << "<> " << provinceIDs.size() << " province definitions registered from:" << mod.name;
			return;
		}

	if (!commonItems::DoesFileExist(EU4Path + "/map/definition.csv"))
		throw std::runtime_error(EU4Path + "/map/definition.csv cannot be found!");

	std::ifstream definitionsFile(EU4Path + "/map/definition.csv");
	parseStream(definitionsFile);
	definitionsFile.close();

	Log(LogLevel::Info) << "<> " << provinceIDs.size() << " province definitions registered.";
}

void EU4::DefinitionScraper::loadDefinitions(std::istream& theStream)
{
	parseStream(theStream);
}

void EU4::DefinitionScraper::parseStream(std::istream& theStream)
{
	std::string line;
	getline(theStream, line); // discard first line.

	while (!theStream.eof())
	{
		getline(theStream, line);
		if (!isdigit(line[0]) || line.length() < 4)
			continue;

		try
		{
			const auto& parsedLineID = parseLine(line);
			if (parsedLineID)
				provinceIDs.insert(*parsedLineID);
			// silently ignore crap we managed to avoid ourselves.
		}
		catch (std::exception& e)
		{
			throw std::runtime_error("Error in definitions.csv line: [" + line + "]! Breaking. (" + e.what() + ")");
		}
	}
}

std::optional<int> EU4::DefinitionScraper::parseLine(const std::string& line)
{
	// We're parsing entire province line in order to make sure it's legit and valid.
	// Mods can come in with broken lines that don't actually load in the game.

	auto sepLoc = line.find(';');
	if (sepLoc == std::string::npos)
		return std::nullopt;
	auto sepLocSave = sepLoc;
	auto ID = std::stoi(line.substr(0, sepLoc));
	sepLoc = line.find(';', sepLocSave + 1);
	if (sepLoc == std::string::npos)
		return std::nullopt;

	// simulate r
	try
	{
		auto r = static_cast<unsigned char>(std::stoi(line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1)));
	}
	catch (std::exception&)
	{
		return std::nullopt;
	}
	sepLocSave = sepLoc;
	sepLoc = line.find(';', sepLocSave + 1);
	if (sepLoc == std::string::npos)
		return std::nullopt;

	// simulate g
	try
	{
		auto g = static_cast<unsigned char>(std::stoi(line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1)));
	}
	catch (std::exception&)
	{
		return std::nullopt;
	}
	sepLocSave = sepLoc;
	sepLoc = line.find(';', sepLocSave + 1);
	if (sepLoc == std::string::npos)
		return std::nullopt;

	// simulate b
	try
	{
		auto b = static_cast<unsigned char>(std::stoi(line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1)));
	}
	catch (std::exception&)
	{
		return std::nullopt;
	}
	sepLocSave = sepLoc;
	sepLoc = line.find(';', sepLocSave + 1);
	if (sepLoc == std::string::npos)
		return std::nullopt;

	// simulate mapDataName
	try
	{
		auto mapDataName = line.substr(sepLocSave + 1, sepLoc - sepLocSave - 1);
	}
	catch (std::exception&)
	{
		return std::nullopt;
	}

	return ID;
}
