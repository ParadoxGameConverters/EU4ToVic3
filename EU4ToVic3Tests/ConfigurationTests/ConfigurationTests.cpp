#include "Configuration.h"
#include "gtest/gtest.h"

TEST(ConfigurationTests, BrokenEU4PathThrowsException)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installationBROKE\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	const commonItems::ConverterVersion converterVersion;

	EXPECT_THROW(const auto configuration = Configuration(configurationInput, converterVersion), std::runtime_error);
}

TEST(ConfigurationTests, BrokenVic3PathThrowsException)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installationBROKE\"\n";
	const commonItems::ConverterVersion converterVersion;

	EXPECT_THROW(const auto configuration = Configuration(configurationInput, converterVersion), std::runtime_error);
}

TEST(ConfigurationTests, InstallationPathsCanBeUpdatedRetrieved)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n"; // updated with "/game/"
	const commonItems::ConverterVersion converterVersion;
	const auto configuration = Configuration(configurationInput, converterVersion);

	EXPECT_EQ(std::filesystem::path("TestFiles/eu4installation"), configuration.getEU4Path());
	EXPECT_EQ(std::filesystem::path("TestFiles/vic3installation/game"), configuration.getVic3Path());
}

TEST(ConfigurationTests, SaveAndDocumentsPathCanBeRetrieved)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	configurationInput << "EU4DocumentsDirectory = \"TestFiles\"\n";
	configurationInput << "SaveGame = \"C:\\autosave.eu4\"\n";
	const commonItems::ConverterVersion converterVersion;
	const auto configuration = Configuration(configurationInput, converterVersion);

	EXPECT_EQ(std::filesystem::path("TestFiles"), configuration.getEU4DocumentsPath());
	EXPECT_EQ(std::filesystem::path("C:\\autosave.eu4"), configuration.getEU4SaveGamePath());
}

TEST(ConfigurationTests, OutputNameNormalizesSetsFromSavegameName)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
#ifdef _MSC_BUILD
	configurationInput << "SaveGame = \"C:\\autosave.eu4\"\n";
#else
	configurationInput << "SaveGame = \"/autosave.eu4\"\n";
#endif
	const commonItems::ConverterVersion converterVersion;
	const auto configuration = Configuration(configurationInput, converterVersion);

	EXPECT_EQ("autosave", configuration.getOutputName().string());
}

TEST(ConfigurationTests, OutputNameNormalizesItselfFromSavegameName)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
#ifdef _MSC_BUILD
	configurationInput << "SaveGame = \"C:\\autosave - something.eu4\"\n";
#else
	configurationInput << "SaveGame = \"/autosave - something.eu4\"\n";
#endif
	const commonItems::ConverterVersion converterVersion;
	const auto configuration = Configuration(configurationInput, converterVersion);

	EXPECT_EQ("autosave___something", configuration.getOutputName().string());
}

TEST(ConfigurationTests, OutputNameSetsFromOverrideName)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	configurationInput << "SaveGame = \"C:\\autosave.eu4\"\n";
	configurationInput << "output_name = \"ddd\"\n";
	const commonItems::ConverterVersion converterVersion;
	const auto configuration = Configuration(configurationInput, converterVersion);

	EXPECT_EQ("ddd", configuration.getOutputName());
}

TEST(ConfigurationTests, OutputNameNormalizesItselfFromOverrideName)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	configurationInput << "SaveGame = \"C:\\autosave - something.eu4\"\n";
	configurationInput << "output_name = \"ddd - something\"\n";
	const commonItems::ConverterVersion converterVersion;
	const auto configuration = Configuration(configurationInput, converterVersion);

	EXPECT_EQ("ddd___something", configuration.getOutputName());
}
