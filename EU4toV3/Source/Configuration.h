#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "Date.h"
#include "GameVersion.h"
#include "Parser.h"
#include <set>
#include <string>

const date HARD_ENDING_DATE("1836.1.1");
const date FUTURE_DATE("2000.1.1");

class Configuration: commonItems::parser
{
  public:
	Configuration();
	explicit Configuration(std::istream& theStream);

	enum class DEADCORES
	{
		LeaveAll = 1,
		DeadCores = 2,
		AllCores = 3
	};
	enum class POPSHAPES
	{
		Vanilla = 1,
		PopShaping = 2,
		Extreme = 3
	};
	enum class COREHANDLES
	{
		DropNone = 1,
		DropNational = 2,
		DropUnions = 3,
		DropAll = 4
	};
	enum class EUROCENTRISM
	{
		EuroCentric = 1,
		VanillaImport = 2
	};

	void setFirstEU4Date(date _firstDate) { firstEU4Date = _firstDate; }
	void setLastEU4Date(date _lastDate) { lastEU4Date = _lastDate; }
	void setStartEU4Date(date _startDate) { startEU4Date = _startDate; }
	void setOutputName(const std::string& name) { outputName = name; }
	void addEU4Mod(const std::string& mod) { EU4Mods.emplace(mod); }
	void setEU4Version(const GameVersion& _version) { version = _version; }
	void setEU4RandomSeed(int seed) { eu4Seed = seed; }

	[[nodiscard]] auto getPopShaping() const { return popShaping; }
	[[nodiscard]] auto getCoreHandling() const { return coreHandling; }
	[[nodiscard]] auto getRemoveType() const { return removeType; }
	[[nodiscard]] auto getEuroCentrism() const { return euroCentric; }
	[[nodiscard]] auto getEU4RandomSeed() const { return eu4Seed; }
	[[nodiscard]] auto getMaxLiteracy() const { return MaxLiteracy; }
	[[nodiscard]] auto getPopShapingFactor() const { return popShapingFactor; }
	[[nodiscard]] auto getConvertAll() const { return convertAll; }

	[[nodiscard]] const auto& getEU4SaveGamePath() const { return EU4SaveGamePath; }
	[[nodiscard]] const auto& getEU4Path() const { return EU4Path; }
	[[nodiscard]] const auto& getEU4DocumentsPath() const { return EU4DocumentsPath; }
	[[nodiscard]] const auto& getVic3Path() const { return Vic3Path; }
	[[nodiscard]] const auto& getVic3DocumentsPath() const { return Vic3DocumentsPath; }
	[[nodiscard]] const auto& getEU4Version() const { return version; }
	[[nodiscard]] const auto& getFirstEU4Date() const { return firstEU4Date; }
	[[nodiscard]] const auto& getLastEU4Date() const { return lastEU4Date; }
	[[nodiscard]] const auto& getStartEU4Date() const { return startEU4Date; }
	[[nodiscard]] const auto& getOutputName() const { return outputName; }
	[[nodiscard]] const auto& getEU4Mods() const { return EU4Mods; }

  private:
	void registerKeys();
	void verifyEU4Path() const;
	void verifyVic3Path();
	void setOutputName();

	// options from configuration.txt
	std::string EU4SaveGamePath;
	std::string EU4Path;
	std::string EU4DocumentsPath;
	std::string Vic3Path;
	std::string Vic3DocumentsPath;
	std::string outputName;

	double MaxLiteracy = 1.0;
	POPSHAPES popShaping = POPSHAPES::Vanilla;
	COREHANDLES coreHandling = COREHANDLES::DropNone;
	DEADCORES removeType = DEADCORES::DeadCores;
	EUROCENTRISM euroCentric = EUROCENTRISM::VanillaImport;
	double popShapingFactor = 50.0;
	bool convertAll = false;

	// items set during conversion
	GameVersion version;
	date firstEU4Date;
	date lastEU4Date;
	date startEU4Date = date("1000.1.1");
	int eu4Seed = 0;
	std::set<std::string> EU4Mods;
};

#endif // CONFIGURATION_H
