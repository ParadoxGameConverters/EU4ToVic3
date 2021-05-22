#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "GameVersion.h"
#include "Date.h"
#include "Parser.h"
#include <string>
#include <vector>

const date HARD_ENDING_DATE("1836.1.1");
const date FUTURE_DATE("2000.1.1");

class Configuration: commonItems::parser
{
	public:
		Configuration() = default;
		Configuration(const Configuration&) = default;
		Configuration(Configuration&&) = default;
		Configuration& operator=(const Configuration&) = default;
		Configuration& operator=(Configuration&&) = default;
		~Configuration() = default;

		void instantiate(std::istream& theStream, bool (*DoesFolderExist)(const std::string& path2), bool (*doesFileExist)(const std::string& path3));

		void setFirstEU4Date(date _firstDate) { firstEU4Date = _firstDate; }
		void setLastEU4Date(date _lastDate) { lastEU4Date = _lastDate; }
		void setStartEU4Date(date _startDate) { startEU4Date = _startDate; }
		void setOutputName(const std::string& name) { outputName = name; }
		void setActualName(const std::string& name) { actualName = name; }
		void addEU4Mod(const std::string& mod) { EU4Mods.push_back(mod); }
		void setEU4Version(const GameVersion& _version) { version = _version; }
		void setEU4RandomSeed(int seed) { eu4Seed = seed; }

		enum class DEADCORES { LeaveAll = 1, DeadCores = 2, AllCores = 3 };
		enum class POPSHAPES { Vanilla = 1, PopShaping = 2, Extreme = 3 };
		enum class COREHANDLES { DropNone = 1, DropNational = 2, DropUnions = 3, DropAll = 4 };
		enum class EUROCENTRISM { EuroCentric = 1, VanillaImport = 2};
		enum class AFRICARESET { ResetAfrica = 1, LeaveAfrica = 2 };
		enum class ABSORBCOLONIES { AbsorbNone = 1, AbsorbSome = 2, AbsorbAll = 3 };
		enum class LIBERTYDESIRE { Loyal = 1, Disloyal = 2, Rebellious = 3 };
		enum class HYBRIDMOD { None = 1, HPM = 2 };

		[[nodiscard]] auto getPopShaping() const { return popShaping; }
		[[nodiscard]] auto getCoreHandling() const { return coreHandling; }
		[[nodiscard]] auto getRemoveType() const { return removeType; }
		[[nodiscard]] auto getEuroCentrism() const { return euroCentric; }
		[[nodiscard]] auto getEU4RandomSeed() const { return eu4Seed; }
		[[nodiscard]] auto getMaxLiteracy() const { return MaxLiteracy; }
		[[nodiscard]] auto getLibertyThreshold() const { return libertyThreshold; }
		[[nodiscard]] auto getPopShapingFactor() const { return popShapingFactor; }
		[[nodiscard]] auto getAbsorbColonies() const { return absorbColonies; }
		[[nodiscard]] auto getDebug() const { return debug; }
		[[nodiscard]] auto getRandomiseRgos() const { return randomiseRgos; }
		[[nodiscard]] auto getConvertAll() const { return convertAll; }
		[[nodiscard]] auto getAfricaReset() const { return africaReset; }
		[[nodiscard]] bool isHpmEnabled() const { return hybridMod == HYBRIDMOD::HPM; }

		[[nodiscard]] const auto& getEU4SaveGamePath() const { return EU4SaveGamePath; }
		[[nodiscard]] const auto& getEU4Path() const { return EU4Path; }
		[[nodiscard]] const auto& getEU4DocumentsPath() const { return EU4DocumentsPath; }
		[[nodiscard]] const auto& getSteamWorkshopPath() const { return SteamWorkshopPath; }
		[[nodiscard]] const auto& getCK2ExportPath() const { return CK2ExportPath; }
		[[nodiscard]] const auto& getVic2Path() const { return Vic2Path; }
		[[nodiscard]] const auto& getVic2DocumentsPath() const { return Vic2DocumentsPath; }
		[[nodiscard]] const auto& getResetProvinces() const { return resetProvinces; }
		[[nodiscard]] const auto& getEU4Version() const { return version; }
		[[nodiscard]] const auto& getFirstEU4Date() const { return firstEU4Date; }
		[[nodiscard]] const auto& getLastEU4Date() const { return lastEU4Date; }
		[[nodiscard]] const auto& getStartEU4Date() const { return startEU4Date; }
		[[nodiscard]] const auto& getOutputName() const { return outputName; }
		[[nodiscard]] const auto& getActualName() const { return actualName; }
		[[nodiscard]] const auto& getEU4Mods() const { return EU4Mods; }

	private:
		static void verifyEU4Path(const std::string& path, bool (*DoesFolderExist)(const std::string& path2), bool (*doesFileExist)(const std::string& path3));
		static void verifyVic2Path(const std::string& path, bool (*DoesFolderExist)(const std::string& path2), bool (*doesFileExist)(const std::string& path3));
		static void verifyVic2DocumentsPath(const std::string& path, bool (*DoesFolderExist)(const std::string& path2));
		void setOutputName();

		// options from configuration.txt
		std::string EU4SaveGamePath;
		std::string EU4Path;
		std::string EU4DocumentsPath;
		std::string SteamWorkshopPath;
		std::string CK2ExportPath;
		std::string Vic2Path;
		std::string Vic2DocumentsPath;
		std::string resetProvinces = "no";
		double MaxLiteracy = 1.0;
		LIBERTYDESIRE libertyThreshold = LIBERTYDESIRE::Loyal;
		POPSHAPES popShaping = POPSHAPES::Vanilla;
		COREHANDLES coreHandling = COREHANDLES::DropNone;
		DEADCORES removeType = DEADCORES::DeadCores;
		EUROCENTRISM euroCentric = EUROCENTRISM::VanillaImport;
		ABSORBCOLONIES absorbColonies = ABSORBCOLONIES::AbsorbNone;
		AFRICARESET africaReset = AFRICARESET::ResetAfrica;
		HYBRIDMOD hybridMod = HYBRIDMOD::None;
		double popShapingFactor = 50.0;
		bool debug = false;
		bool randomiseRgos = false;
		bool convertAll = false;
	
		// items set during conversion
		GameVersion version;
		date firstEU4Date;
		date lastEU4Date;
		date startEU4Date = date("1000.1.1");
		int eu4Seed = 0;
		std::string outputName;  // actual output name
		std::string incomingOutputName; // from configuration file
		std::string actualName; // Not normalized like outputName
		std::vector<std::string> EU4Mods;
};

extern Configuration theConfiguration;

class ConfigurationFile: commonItems::parser
{
	public:
		explicit ConfigurationFile(const std::string& filename);
		~ConfigurationFile() = default;
		ConfigurationFile() = delete;
		ConfigurationFile(const ConfigurationFile&) = delete;
		ConfigurationFile(ConfigurationFile&&) = delete;
		ConfigurationFile& operator=(const ConfigurationFile&) = delete;
		ConfigurationFile& operator=(ConfigurationFile&&) = delete;
};

#endif // CONFIGURATION_H
