#include "ModLoader.h"
#include "CommonFunctions.h"
#include "Configuration.h"
#include "Log.h"
#include "ModParser.h"
#include "OSCompatibilityLayer.h"
#include "ZipFile.h"
#include <filesystem>
#include <fstream>
#include <ranges>
#include <set>
#include <stdexcept>
#include <string>
namespace fs = std::filesystem;

void EU4::ModLoader::loadMods(const Configuration& configuration, const Mods& incomingMods)
{
	if (incomingMods.empty())
	{
		// We shouldn't even be here if the save didn't have mods! Why were Mods called?
		Log(LogLevel::Info) << "No mods were detected in savegame. Skipping mod processing.";
		return;
	}

	// We enter this function with a map of mod names and mod file locations from the savegame. We need to read all the
	// mod files, check their paths (and potential archives for ancient mods) unpack what's necessary, and exit
	// with a map of updated mod names (savegame can differ from actual mod file) and mod folder locations.

	// This function reads all the incoming mod files and verify their internal paths/archives are correct and point to something present on disk.
	loadEU4ModDirectory(configuration, incomingMods);

	Log(LogLevel::Info) << "\tDetermining Mod Usability";
	auto allMods = possibleMods;
	allMods.insert(possibleCompressedMods.begin(), possibleCompressedMods.end());
	for (const auto& usedModName: std::views::keys(allMods))
	{
		// This invocation (getModPath) will unpack any compressed mods into out converter's folder.
		if (auto possibleModPath = getModPath(usedModName); possibleModPath)
		{
			if (!commonItems::DoesFolderExist(*possibleModPath) && !commonItems::DoesFileExist(*possibleModPath))
			{
				Log(LogLevel::Warning) << "\t\t" << usedModName << " could not be found in the specified mod directory. Tried: " << *possibleModPath;
				continue;
			}

			// All verified mods go into usableMods
			Log(LogLevel::Info) << "\t\t->> Found potentially useful [" << usedModName << "]: " << *possibleModPath + "/";
			usableMods.insert(std::pair(usedModName, *possibleModPath + "/"));
		}
		else
		{
			Log(LogLevel::Warning) << "\t\tNo path could be found for " << usedModName
										  << ". Check that the mod is present and that the .mod file specifies the path for the mod";
		}
	}
}

void EU4::ModLoader::loadEU4ModDirectory(const Configuration& configuration, const Mods& incomingMods)
{
	const auto& EU4ModsPath = configuration.getEU4DocumentsPath() + "/mod";
	if (!commonItems::DoesFolderExist(EU4ModsPath))
		throw std::invalid_argument("EU4 mods directory path is invalid! Is it at: " + configuration.getEU4DocumentsPath() + "/mod/ ?");

	Log(LogLevel::Info) << "\tEU4 mods directory is " << EU4ModsPath;

	const auto diskModNames = commonItems::GetAllFilesInFolder(EU4ModsPath);
	for (const auto& [usedModName, usedModFilePath]: incomingMods)
	{
		const auto trimmedModFileName = trimPath(usedModFilePath);
		if (!diskModNames.contains(trimmedModFileName))
		{
			Log(LogLevel::Warning) << "\t\tSavegame uses " << usedModName << " at " << usedModFilePath
										  << " which is not present on disk.  Skipping at your risk, but this can greatly affect conversion.";
			continue;
		}
		if (getExtension(trimmedModFileName) != "mod")
			continue; // shouldn't be necessary but just in case.
		try
		{
			std::ifstream modFile(fs::u8path(EU4ModsPath + "/" + trimmedModFileName));
			ModParser theMod(modFile);
			modFile.close();

			if (!theMod.isValid())
			{
				Log(LogLevel::Warning) << "\t\tMod at " << EU4ModsPath + "/" + trimmedModFileName << " does not look valid.";
				continue;
			}

			if (!theMod.isCompressed())
			{
				if (!commonItems::DoesFolderExist(theMod.getPath()))
				{
					// Maybe we have a relative path
					if (commonItems::DoesFolderExist(configuration.getEU4DocumentsPath() + "/" + theMod.getPath()))
					{
						// fix this.
						theMod.setPath(configuration.getEU4DocumentsPath() + "/" + theMod.getPath());
					}
					else
					{
						Log(LogLevel::Warning) << "\t\tMod " << usedModName
													  << " at " + usedModFilePath + " points to " + theMod.getPath() +
																" which does not exist! Skipping at your risk, but this can greatly affect conversion.";
						continue;
					}
				}

				possibleMods.insert(std::make_pair(theMod.getName(), theMod.getPath()));
				Log(LogLevel::Info) << "\t\tFound potential mod named " << theMod.getName() << " with a mod file at " << EU4ModsPath + "/" + trimmedModFileName
										  << " and itself at " << theMod.getPath();
			}
			else
			{
				// Maybe we have a relative path
				if (commonItems::DoesFileExist(configuration.getEU4DocumentsPath() + "/" + theMod.getPath()))
				{
					// fix this.
					theMod.setPath(configuration.getEU4DocumentsPath() + "/" + theMod.getPath());
				}
				else
				{
					if (!commonItems::DoesFileExist(theMod.getPath()))
					{
						Log(LogLevel::Warning) << "\t\tMod " << usedModName
													  << " at " + usedModFilePath + " points to " + theMod.getPath() +
																" which does not exist! Skipping at your risk, but this can greatly affect conversion.";
						continue;
					}
				}
				possibleCompressedMods.insert(std::make_pair(theMod.getName(), theMod.getPath()));
				Log(LogLevel::Info) << "\t\tFound a compressed mod named " << theMod.getName() << " with a mod file at " << EU4ModsPath << "/"
										  << trimmedModFileName << " and itself at " << theMod.getPath();
			}
		}
		catch (std::exception&)
		{
			Log(LogLevel::Warning) << "Error while reading " << EU4ModsPath << "/" << trimmedModFileName << "! Mod will not be useable for conversions.";
		}
	}
}

std::optional<std::string> EU4::ModLoader::getModPath(const std::string& modName) const
{
	if (const auto& mod = possibleMods.find(modName); mod != possibleMods.end())
	{
		return mod->second;
	}

	if (const auto& compressedMod = possibleCompressedMods.find(modName); compressedMod != possibleCompressedMods.end())
	{
		const auto archivePath = compressedMod->second;
		const auto uncompressedName = trimPath(trimExtension(archivePath));

		commonItems::TryCreateFolder("mods/");

		if (!commonItems::DoesFolderExist("mods/" + uncompressedName))
		{
			Log(LogLevel::Info) << "\t\tUncompressing: " << archivePath;
			if (!extractZip(archivePath, "mods/" + uncompressedName))
			{
				Log(LogLevel::Warning) << "We're having trouble automatically uncompressing your mod.";
				Log(LogLevel::Warning) << "Please, manually uncompress: " << archivePath;
				Log(LogLevel::Warning) << "Into: EU4ToVic3/mods/" << uncompressedName;
				Log(LogLevel::Warning) << "Then run the converter again. Thank you and good luck.";
				return std::nullopt;
			}
		}

		if (commonItems::DoesFolderExist("mods/" + uncompressedName))
		{
			return "mods/" + uncompressedName;
		}
	}

	return std::nullopt;
}

bool EU4::ModLoader::extractZip(const std::string& archive, const std::string& path) const
{
	commonItems::TryCreateFolder(path);
	auto modFile = ZipFile::Open(archive);
	if (!modFile)
		return false;
	for (size_t entryNum = 0; entryNum < modFile->GetEntriesCount(); ++entryNum)
	{
		const auto& entry = modFile->GetEntry(static_cast<int>(entryNum));
		const auto& inPath = entry->GetFullName();
		const auto& name = entry->GetName();
		if (entry->IsDirectory())
			continue;

		// Does target directory exist?
		const auto dirNamePos = inPath.find(name);

		if (const auto dirName = path + "/" + inPath.substr(0, dirNamePos); !commonItems::DoesFolderExist(dirName))
		{
			// we need to craft our way through to target directory.
			auto remainder = inPath;
			auto currentPath = path;
			while (remainder != name)
			{
				if (const auto pos = remainder.find_first_of('/'); pos != std::string::npos)
				{
					auto makeDirName = remainder.substr(0, pos);
					currentPath += "/" + makeDirName;
					commonItems::TryCreateFolder(currentPath);
					remainder = remainder.substr(pos + 1, remainder.length());
				}
				else
					break;
			}
		}
		ZipFile::ExtractFile(archive, inPath, path + "/" + inPath);
	}
	return true;
}