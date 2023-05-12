#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "ConverterVersion.h"
#include "Parser.h"
#include <string>

class Configuration: commonItems::parser
{
  public:
	Configuration() = default;
	explicit Configuration(const commonItems::ConverterVersion& converterVersion);
	explicit Configuration(std::istream& theStream, const commonItems::ConverterVersion& converterVersion);

	enum class STARTDATE
	{
		Vanilla = 1,
		Dynamic = 2
	};
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
	enum class EUROCENTRISM
	{
		EuroCentric = 1,
		VanillaImport = 2
	};
	enum class ECONOMY
	{
		CivLevel = 1,
		DevPopVanilla = 2,
		DevPopActual = 3,
		Test = 9
	};
	enum class SPLITTCS
	{
		TradeRegions = 1,
		SuperRegions = 2,
		Disabled = 3
	};

	struct ConfigBlock
	{
		STARTDATE startDate = STARTDATE::Vanilla;
		POPSHAPES popShaping = POPSHAPES::Vanilla;
		DEADCORES removeType = DEADCORES::DeadCores;
		EUROCENTRISM euroCentric = EUROCENTRISM::VanillaImport;
		ECONOMY economy = ECONOMY::CivLevel;
		SPLITTCS splitTCs = SPLITTCS::SuperRegions;
		bool convertAll = false;
		bool downTiers = true;
		// runtime options.
		bool vn = false; // Voltaire's Nightmare
	} configBlock;

	void setOutputName(const std::string& name) { outputName = name; }
	void setVN() { configBlock.vn = true; }
	void setEurocentric() { configBlock.euroCentric = EUROCENTRISM::EuroCentric; }
	void setVanillaStartDate() { configBlock.startDate = STARTDATE::Vanilla; }

	[[nodiscard]] const auto& getEU4SaveGamePath() const { return EU4SaveGamePath; }
	[[nodiscard]] const auto& getEU4Path() const { return EU4Path; }
	[[nodiscard]] const auto& getEU4DocumentsPath() const { return EU4DocumentsPath; }
	[[nodiscard]] const auto& getVic3Path() const { return Vic3Path; }
	[[nodiscard]] const auto& getOutputName() const { return outputName; }

  private:
	void registerKeys();
	void verifyEU4Path() const;
	void verifyVic3Path();
	void setOutputName();
	void verifyVic3Version(const commonItems::ConverterVersion& converterVersion) const;
	void verifyEU4Version(const commonItems::ConverterVersion& converterVersion) const;

	// options from configuration.txt
	std::string EU4SaveGamePath;
	std::string EU4Path;
	std::string EU4DocumentsPath;
	std::string Vic3Path;
	std::string outputName;
};

#endif // CONFIGURATION_H
