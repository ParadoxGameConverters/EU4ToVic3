#ifndef CONFIGURATION_H
#define CONFIGURATION_H
#include "Date.h"
#include "Parser.h"
#include <string>

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

	struct ConfigBlock
	{
		double MaxLiteracy = 1.0;
		POPSHAPES popShaping = POPSHAPES::Vanilla;
		COREHANDLES coreHandling = COREHANDLES::DropNone;
		DEADCORES removeType = DEADCORES::DeadCores;
		EUROCENTRISM euroCentric = EUROCENTRISM::VanillaImport;
		double popShapingFactor = 50.0;
		bool convertAll = false;
	} configBlock;

	void setOutputName(const std::string& name) { outputName = name; }

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

	// options from configuration.txt
	std::string EU4SaveGamePath;
	std::string EU4Path;
	std::string EU4DocumentsPath;
	std::string Vic3Path;
	std::string outputName;
};

#endif // CONFIGURATION_H
