#include "Configuration.h"
#include "OSCompatibilityLayer.h"
#include "gtest/gtest.h"

TEST(ConfigurationTests, InstallationPathsCanBeVerified)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";

	EXPECT_NO_THROW(const auto configuration = Configuration(configurationInput));
	EXPECT_TRUE(commonItems::DoesFolderExist("TestFiles/eu4installation"));
	EXPECT_TRUE(commonItems::DoesFolderExist("TestFiles/vic3installation"));
}

TEST(ConfigurationTests, BrokenEU4PathThrowsException)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installationBROKE\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";

	EXPECT_THROW(const auto configuration = Configuration(configurationInput), std::runtime_error);
}

TEST(ConfigurationTests, BrokenVic3PathThrowsException)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installationBROKE\"\n";

	EXPECT_THROW(const auto configuration = Configuration(configurationInput), std::runtime_error);
}

TEST(ConfigurationTests, InstallationPathsCanBeUpdatedRetrieved)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n"; // updated with "/game/"
	const auto configuration = Configuration(configurationInput);

	EXPECT_EQ("TestFiles/eu4installation", configuration.getEU4Path());
	EXPECT_EQ("TestFiles/vic3installation/game/", configuration.getVic3Path());
}

TEST(ConfigurationTests, SaveAndDocumentsPathCanBeRetrieved)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	configurationInput << "EU4DocumentsDirectory = \"TestFiles\"\n";
	configurationInput << "SaveGame = \"C:\\autosave.eu4\"\n";
	const auto configuration = Configuration(configurationInput);

	EXPECT_EQ("TestFiles", configuration.getEU4DocumentsPath());
	EXPECT_EQ("C:\\autosave.eu4", configuration.getEU4SaveGamePath());
}

TEST(ConfigurationTests, OutputNameNormalizesSetsFromSavegameName)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	configurationInput << "SaveGame = \"C:\\autosave.eu4\"\n";
	const auto configuration = Configuration(configurationInput);

	EXPECT_EQ("autosave", configuration.getOutputName());
}

TEST(ConfigurationTests, OutputNameNormalizesItselfFromSavegameName)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	configurationInput << "SaveGame = \"C:\\autosave - something.eu4\"\n";
	const auto configuration = Configuration(configurationInput);

	EXPECT_EQ("autosave___something", configuration.getOutputName());
}

TEST(ConfigurationTests, OutputNameSetsFromOverrideName)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	configurationInput << "SaveGame = \"C:\\autosave.eu4\"\n";
	configurationInput << "output_name = \"ddd\"\n";
	const auto configuration = Configuration(configurationInput);

	EXPECT_EQ("ddd", configuration.getOutputName());
}

TEST(ConfigurationTests, OutputNameNormalizesItselfFromOverrideName)
{
	std::stringstream configurationInput;
	configurationInput << "EU4directory = \"TestFiles/eu4installation\"\n";
	configurationInput << "Vic3directory = \"TestFiles/vic3installation\"\n";
	configurationInput << "SaveGame = \"C:\\autosave - something.eu4\"\n";
	configurationInput << "output_name = \"ddd - something\"\n";
	const auto configuration = Configuration(configurationInput);

	EXPECT_EQ("ddd___something", configuration.getOutputName());
}
