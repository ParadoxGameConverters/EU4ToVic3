#include "CommonCountryLoader/CommonCountryLoader.h"
#include "Configuration.h"
#include "ConverterVersion.h"
#include "gtest/gtest.h"

TEST(EU4World_CommonCountryLoaderTests, colorsCanBeLoadedFromCommonCountries)
{
	std::stringstream configurationInput;
	configurationInput << "EU4DocumentsDirectory = \"TestFiles\"\n";
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	const commonItems::ConverterVersion converterVersion;
	const auto configuration = Configuration(configurationInput, converterVersion);

	Mods mods;
	mods.emplace_back(Mod("Some mod", "mod/themod.mod"));

	commonItems::ModLoader modLoader;
	modLoader.loadMods(configuration.getEU4DocumentsPath(), mods);
	mods = modLoader.getMods();

	auto modFS = commonItems::ModFilesystem("TestFiles/eu4installation/", mods);

	EU4::CommonCountryLoader loader;
	loader.loadCommonCountries(modFS);

	EXPECT_EQ("= rgb { 157 51 167 }", loader.getCommonColor("AAA")->outputRgb());
	EXPECT_EQ("= rgb { 49 115 90 }", loader.getCommonColor("BBB")->outputRgb());
	EXPECT_EQ("= rgb { 18 139 228 }", loader.getCommonColor("CCC")->outputRgb());
	EXPECT_EQ(std::nullopt, loader.getCommonColor("DDD"));
}
