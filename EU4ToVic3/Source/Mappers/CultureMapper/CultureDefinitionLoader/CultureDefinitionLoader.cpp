#include "CultureDefinitionLoader.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "CultureDefinitionEntry.h"
#include "Log.h"
#include "ParserHelpers.h"
#include <array>
#include <string_view>

namespace
{
using Operation = mappers::CultureDefinitionOperation;

constexpr std::array<std::pair<std::string_view, Operation>, 6> operationPrefixes{{
	 {"INJECT:", Operation::Inject},
	 {"REPLACE:", Operation::Replace},
	 {"TRY_INJECT:", Operation::TryInject},
	 {"TRY_REPLACE:", Operation::TryReplace},
	 {"INJECT_OR_CREATE:", Operation::InjectOrCreate},
	 {"REPLACE_OR_CREATE:", Operation::ReplaceOrCreate},
}};

std::pair<Operation, std::string> determineOperationAndName(const std::string& cultureNameStr)
{
	for (const auto& [prefix, operation]: operationPrefixes)
	{
		if (cultureNameStr.rfind(prefix.data(), 0) == 0)
			return {operation, cultureNameStr.substr(prefix.size())};
	}

	return {Operation::Assign, cultureNameStr};
}

const char* toString(const Operation operation)
{
	switch (operation)
	{
		case Operation::Assign:
			return "assignment";
		case Operation::Inject:
			return "INJECT";
		case Operation::Replace:
			return "REPLACE";
		case Operation::TryInject:
			return "TRY_INJECT";
		case Operation::TryReplace:
			return "TRY_REPLACE";
		case Operation::InjectOrCreate:
			return "INJECT_OR_CREATE";
		case Operation::ReplaceOrCreate:
			return "REPLACE_OR_CREATE";
	}

	return "unknown operation";
}

void mergeLocalizations(std::map<std::string, std::string>& target, const std::map<std::string, std::string>& source)
{
	for (const auto& [key, value]: source)
		target.insert_or_assign(key, value);
}

void injectCultureDefinition(mappers::CultureDef& target, const mappers::CultureDef& source)
{
	if (source.hasColor)
		target.color = source.color;
	if (source.hasReligion)
		target.religion = source.religion;
	if (source.hasLanguage)
		target.language = source.language;
	if (source.hasHeritage)
		target.heritage = source.heritage;
	if (source.hasGraphics)
		target.graphics = source.graphics;

	target.obsessions.insert(source.obsessions.begin(), source.obsessions.end());
	target.traditions.insert(source.traditions.begin(), source.traditions.end());
	target.maleCommonFirstNames.insert(source.maleCommonFirstNames.begin(), source.maleCommonFirstNames.end());
	target.femaleCommonFirstNames.insert(source.femaleCommonFirstNames.begin(), source.femaleCommonFirstNames.end());
	target.commonLastNames.insert(source.commonLastNames.begin(), source.commonLastNames.end());
	target.nobleLastNames.insert(source.nobleLastNames.begin(), source.nobleLastNames.end());
	target.maleRegalFirstNames.insert(source.maleRegalFirstNames.begin(), source.maleRegalFirstNames.end());
	target.femaleRegalFirstNames.insert(source.femaleRegalFirstNames.begin(), source.femaleRegalFirstNames.end());
	target.regalLastNames.insert(source.regalLastNames.begin(), source.regalLastNames.end());
	target.ethnicities.insert(source.ethnicities.begin(), source.ethnicities.end());

	target.skipProcessing = source.skipProcessing;
	target.skipExport = source.skipExport;
	mergeLocalizations(target.locBlock, source.locBlock);
	mergeLocalizations(target.nameLocBlock, source.nameLocBlock);
	target.operation = Operation::Assign;
}

void materializeCultureDefinition(mappers::CultureDef& cultureDefinition)
{
	cultureDefinition.operation = Operation::Assign;
}
} // namespace

void mappers::CultureDefinitionLoader::applyDefinition(CultureDef cultureDefinition)
{
	const auto cultureName = cultureDefinition.name;
	const auto operation = cultureDefinition.operation;
	auto existingDefinition = cultureDefinitions.find(cultureName);

	switch (operation)
	{
		case Operation::Assign:
		case Operation::ReplaceOrCreate:
			materializeCultureDefinition(cultureDefinition);
			cultureDefinitions.insert_or_assign(cultureName, std::move(cultureDefinition));
			return;
		case Operation::InjectOrCreate:
			if (existingDefinition == cultureDefinitions.end())
			{
				materializeCultureDefinition(cultureDefinition);
				cultureDefinitions.emplace(cultureName, std::move(cultureDefinition));
			}
			else
			{
				injectCultureDefinition(existingDefinition->second, cultureDefinition);
			}
			return;
		case Operation::Inject:
			if (existingDefinition == cultureDefinitions.end())
			{
				Log(LogLevel::Warning) << toString(operation) << " requested for missing culture definition " << cultureName << ".";
				return;
			}
			injectCultureDefinition(existingDefinition->second, cultureDefinition);
			return;
		case Operation::TryInject:
			if (existingDefinition != cultureDefinitions.end())
				injectCultureDefinition(existingDefinition->second, cultureDefinition);
			return;
		case Operation::Replace:
			if (existingDefinition == cultureDefinitions.end())
			{
				Log(LogLevel::Warning) << toString(operation) << " requested for missing culture definition " << cultureName << ".";
				return;
			}
			materializeCultureDefinition(cultureDefinition);
			existingDefinition->second = std::move(cultureDefinition);
			return;
		case Operation::TryReplace:
			if (existingDefinition != cultureDefinitions.end())
			{
				materializeCultureDefinition(cultureDefinition);
				existingDefinition->second = std::move(cultureDefinition);
			}
			return;
	}
}

void mappers::CultureDefinitionLoader::loadDefinitions(const commonItems::ModFilesystem& modFS)
{
	Log(LogLevel::Info) << "-> Loading culture definitions.";
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("common/cultures"))
	{
		if (fileName.extension() != ".txt")
			continue;

		if (fileName.string().find("99_") != std::string::npos)
			skipProcessing = false;
		else
			skipProcessing = true;

		if (fileName.string().find("blankMod") != std::string::npos)
			skipExport = true;
		else
			skipExport = false;

		parseFile(fileName);
	}
	clearRegisteredKeywords();
	Log(LogLevel::Info) << "<> " << cultureDefinitions.size() << " definitions loaded.";
}

void mappers::CultureDefinitionLoader::loadDefinitions(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureDefinitionLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& cultureNameStr, std::istream& theStream) {
		const auto [operation, cultureName] = determineOperationAndName(cultureNameStr);
		auto relDef = CultureDefinitionEntry(theStream, skipProcessing, skipExport).getCultureDef();
		relDef.operation = operation;
		relDef.name = cultureName;
		applyDefinition(std::move(relDef));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
