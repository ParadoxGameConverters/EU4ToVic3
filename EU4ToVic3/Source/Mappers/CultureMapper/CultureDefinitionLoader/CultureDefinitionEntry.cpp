#include "CultureDefinitionEntry.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"
#include "ParserHelpers.h"
#include <ranges>

namespace
{
std::string normalizeString(const std::string& input)
{
	auto toReturn = commonItems::normalizeUTF8Path(input);
	std::replace(toReturn.begin(), toReturn.end(), ' ', '_');
	std::replace(toReturn.begin(), toReturn.end(), '\'', '_');
	return toReturn;
}
} // namespace

mappers::CultureDefinitionEntry::CultureDefinitionEntry(std::istream& theStream, bool skipProcessing, bool skipExport)
{
	cultureDef.skipProcessing = skipProcessing;
	cultureDef.skipExport = skipExport;
	registerkeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void mappers::CultureDefinitionEntry::registerkeys()
{
	ethStripper.registerRegex(R"(\d+)", [this](const std::string& unused, std::istream& theStream) {
		cultureDef.ethnicities.emplace(commonItems::getString(theStream));
	});
	ethStripper.registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);

	registerKeyword("color", [this](std::istream& theStream) {
		cultureDef.color = commonItems::Color::Factory().getColor(theStream);
	});
	registerKeyword("religion", [this](std::istream& theStream) {
		cultureDef.religion = commonItems::getString(theStream);
	});
	registerKeyword("traits", [this](std::istream& theStream) {
		for (const auto& trait: commonItems::getStrings(theStream))
			cultureDef.traits.emplace(trait);
	});
	registerKeyword("male_common_first_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
		{
			if (cultureDef.skipProcessing)
				cultureDef.maleCommonFirstNames.emplace(name);
			else
			{
				auto dwName = "dw_" + normalizeString(name);
				cultureDef.maleCommonFirstNames.emplace(dwName);
				cultureDef.nameLocBlock.emplace(dwName, name);
			}
		}
	});
	registerKeyword("female_common_first_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
		{
			if (cultureDef.skipProcessing)
				cultureDef.femaleCommonFirstNames.emplace(name);
			else
			{
				auto dwName = "dw_" + normalizeString(name);
				cultureDef.femaleCommonFirstNames.emplace(dwName);
				cultureDef.nameLocBlock.emplace(dwName, name);
			}
		}
	});
	registerKeyword("noble_last_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
		{
			if (cultureDef.skipProcessing)
				cultureDef.nobleLastNames.emplace(name);
			else
			{
				auto dwName = "dw_" + normalizeString(name);
				cultureDef.nobleLastNames.emplace(dwName);
				cultureDef.nameLocBlock.emplace(dwName, name);
			}
		}
	});
	registerKeyword("common_last_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
		{
			if (cultureDef.skipProcessing)
				cultureDef.commonLastNames.emplace(name);
			else
			{
				auto dwName = "dw_" + normalizeString(name);
				cultureDef.commonLastNames.emplace(dwName);
				cultureDef.nameLocBlock.emplace(dwName, name);
			}
		}
	});
	registerKeyword("male_regal_first_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
		{
			if (cultureDef.skipProcessing)
				cultureDef.maleRegalFirstNames.emplace(name);
			else
			{
				auto dwName = "dw_" + normalizeString(name);
				cultureDef.maleRegalFirstNames.emplace(dwName);
				cultureDef.nameLocBlock.emplace(dwName, name);
			}
		}
	});
	registerKeyword("female_regal_first_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
		{
			if (cultureDef.skipProcessing)
				cultureDef.femaleRegalFirstNames.emplace(name);
			else
			{
				auto dwName = "dw_" + normalizeString(name);
				cultureDef.femaleRegalFirstNames.emplace(dwName);
				cultureDef.nameLocBlock.emplace(dwName, name);
			}
		}
	});
	registerKeyword("regal_last_names", [this](std::istream& theStream) {
		for (const auto& name: commonItems::getStrings(theStream))
		{
			if (cultureDef.skipProcessing)
				cultureDef.regalLastNames.emplace(name);
			else
			{
				auto dwName = "dw_" + normalizeString(name);
				cultureDef.regalLastNames.emplace(dwName);
				cultureDef.nameLocBlock.emplace(dwName, name);
			}
		}
	});
	registerKeyword("ethnicities", [this](std::istream& theStream) {
		ethStripper.parseStream(theStream);
	});
	registerKeyword("graphics", [this](std::istream& theStream) {
		cultureDef.graphics = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
