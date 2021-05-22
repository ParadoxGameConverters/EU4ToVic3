/*Copyright (c) 2019 The Paradox Game Converters Project

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.*/



#include "gtest/gtest.h"
#include "../EU4toV3/Source/Configuration.h"
#include <sstream>


bool fakeDoesFolderExist(const std::string& folder)
{
	if (folder == "C:\\EU4PathWithoutExecutable")
	{
		return true;
	}
	else if (folder == "C:\\EU4PathWithoutDataFile")
	{
		return true;
	}
	else if (folder == "C:\\EU4Path")
	{
		return true;
	}
	else if (folder == "C:\\Vic2PathWithoutExecutable")
	{
		return true;
	}
	else if (folder == "C:\\Vic2Path")
	{
		return true;
	}
	else if (folder == "C:\\Vic2DocumentsPath")
	{
		return true;
	}
	else if (folder == "C:\\Vic2Path/mod/HPM")
	{
		return true;
	}
	else
	{
		return false;
	}
}


bool fakeDoesFileExist(const std::string& file)
{
	if (file == "C:\\EU4PathWithoutDataFile/eu4.exe")
	{
		return true;
	}
	else if (file == "C:\\EU4Path/eu4.exe")
	{
		return true;
	}
	else if (file == "C:\\EU4Path/map/positions.txt")
	{
		return true;
	}
	else if (file == "C:\\Vic2Path/v2game.exe")
	{
		return true;
	}
	return false;
}



TEST(EU4ToVic2_ConfigurationTests, EU4PathDefaultsBlank)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getEU4Path(), "");
}


TEST(EU4ToVic2_ConfigurationTests, EU4PathThatDoesntExistFails)
{
	Configuration testConfiguration;
	std::stringstream input("EU4directory = \"C:\\FakeDirectory\"");

	try
	{
		testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);
		FAIL();
	}
	catch( const std::runtime_error& e )
	{
	   ASSERT_STREQ( "C:\\FakeDirectory does not exist!", e.what() );
	}
}


TEST(EU4ToVic2_ConfigurationTests, EU4PathWithoutExecutableFails)
{
	Configuration testConfiguration;
	std::stringstream input("EU4directory = \"C:\\EU4PathWithoutExecutable\"");

	try
	{
		testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);
		FAIL();
	}
	catch( const std::runtime_error& e )
	{
	   ASSERT_STREQ( "C:\\EU4PathWithoutExecutable does not contain Europa Universalis 4!", e.what() );
	}
}


TEST(EU4ToVic2_ConfigurationTests, EU4PathWithoutDataFileFails)
{
	Configuration testConfiguration;
	std::stringstream input("EU4directory = \"C:\\EU4PathWithoutDataFile\"");

	try
	{
		testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);
		FAIL();
	}
	catch( const std::runtime_error& e )
	{
	   ASSERT_STREQ( "C:\\EU4PathWithoutDataFile does not appear to be a valid EU4 install!", e.what() );
	}
}


TEST(EU4ToVic2_ConfigurationTests, EU4PathCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("EU4directory = \"C:\\EU4Path\"");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getEU4Path(), "C:\\EU4Path");
}


TEST(EU4ToVic2_ConfigurationTests, EU4DocumentsDirectoryDefaultsBlank)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getEU4DocumentsPath(), "");
}


TEST(EU4ToVic2_ConfigurationTests, EU4DocumentsDirectoryCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("EU4DocumentsDirectory = \"C:\\EU4DocumentsDirectory\"");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getEU4DocumentsPath(), "C:\\EU4DocumentsDirectory");
}


TEST(EU4ToVic2_ConfigurationTests, SteamWorkshopDirectoryDefaultsBlank)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getSteamWorkshopPath(), "");
}


TEST(EU4ToVic2_ConfigurationTests, SteamWorkshopDirectoryCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("SteamWorkshopDirectory = \"C:\\SteamWorksopDirectory\"");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getSteamWorkshopPath(), "C:\\SteamWorksopDirectory");
}


TEST(EU4ToVic2_ConfigurationTests, CK2ExportDirectoryDefaultsBlank)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getCK2ExportPath(), "");
}


TEST(EU4ToVic2_ConfigurationTests, CK2ExportDirectoryCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("CK2ExportDirectory = \"C:\\CK2ExportDirectory\"");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getCK2ExportPath(), "C:\\CK2ExportDirectory");
}


TEST(EU4ToVic2_ConfigurationTests, Vic2PathDefaultsBlank)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getVic2Path(), "");
}


TEST(EU4ToVic2_ConfigurationTests, Vic2PathThatDoesntExistFails)
{
	Configuration testConfiguration;
	std::stringstream input("Vic2directory = \"C:\\FakeDirectory\"");

	try
	{
		testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);
		FAIL();
	}
	catch( const std::runtime_error& e )
	{
	   ASSERT_STREQ( "C:\\FakeDirectory does not exist!", e.what() );
	}
}


TEST(EU4ToVic2_ConfigurationTests, Vic2PathWithoutExecutableFails)
{
	Configuration testConfiguration;
	std::stringstream input("Vic2directory = \"C:\\Vic2PathWithoutExecutable\"");

	try
	{
		testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);
		FAIL();
	}
	catch( const std::runtime_error& e )
	{
	   ASSERT_STREQ( "C:\\Vic2PathWithoutExecutable does not contain Victoria 2!", e.what() );
	}
}


TEST(EU4ToVic2_ConfigurationTests, Vic2PathCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("Vic2directory = \"C:\\Vic2Path\"");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getVic2Path(), "C:\\Vic2Path");
}


TEST(EU4ToVic2_ConfigurationTests, Vic2PathCanPointToHpm)
{
	Configuration testConfiguration;
	std::stringstream input;
	input << "Vic2directory = \"C:\\Vic2Path\"";
	input << "hybrid_mod = \"2\"";
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ("C:\\Vic2Path/mod/HPM", testConfiguration.getVic2Path());
}


TEST(EU4ToVic2_ConfigurationTests, Vic2PathDefaultsToVanilla)
{
	Configuration testConfiguration;
	std::stringstream input;
	input << "Vic2directory = \"C:\\Vic2Path\"";
	input << "hpm_enabled = \"1\"";
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ("C:\\Vic2Path", testConfiguration.getVic2Path());
}


TEST(EU4ToVic2_ConfigurationTests, Vic2DocumentsPathDefaultsBlank)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getVic2DocumentsPath(), "");
}


TEST(EU4ToVic2_ConfigurationTests, Vic2DocumentsPathThatDoesntExistFails)
{
	Configuration testConfiguration;
	std::stringstream input("Vic2Documentsdirectory = \"C:\\FakeDirectory\"");

	try
	{
		testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);
		FAIL();
	}
	catch( const std::runtime_error& e )
	{
	   // and this tests that it has the correct message
	   ASSERT_STREQ( "C:\\FakeDirectory does not exist!", e.what() );
	}
}


TEST(EU4ToVic2_ConfigurationTests, Vic2DocumentsPathCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("Vic2Documentsdirectory = \"C:\\Vic2DocumentsPath\"");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getVic2DocumentsPath(), "C:\\Vic2DocumentsPath");
}


TEST(EU4ToVic2_ConfigurationTests, ResetProvincesDefaultsNo)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getResetProvinces(), "no");
}


TEST(EU4ToVic2_ConfigurationTests, ResetProvincesCannotBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("resetProvinces = yes");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getResetProvinces(), "no");
}


TEST(EU4ToVic2_ConfigurationTests, MaxLiteracyDefaultsToOne)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getMaxLiteracy(), 1.0);
}


TEST(EU4ToVic2_ConfigurationTests, MaxLiteracyCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("max_literacy = 50");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getMaxLiteracy(), 0.5);
}


TEST(EU4ToVic2_ConfigurationTests, RemoveTypeDefaultsToDead)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getRemoveType(), Configuration::DEADCORES::DeadCores);
}


TEST(EU4ToVic2_ConfigurationTests, RemoveTypeCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("remove_type = 1");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getRemoveType(), Configuration::DEADCORES::LeaveAll);
}


TEST(EU4ToVic2_ConfigurationTests, LibertyThresholdDefaultsToFifty)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getLibertyThreshold(), Configuration::LIBERTYDESIRE::Loyal);
}


TEST(EU4ToVic2_ConfigurationTests, LibertyThresholdCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("liberty_threshold = 2");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getLibertyThreshold(), Configuration::LIBERTYDESIRE::Disloyal);
}


TEST(EU4ToVic2_ConfigurationTests, PopShapingDefaultsToVanilla)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getPopShaping(), Configuration::POPSHAPES::Vanilla);
}


TEST(EU4ToVic2_ConfigurationTests, PopShapingCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("pop_shaping = 2");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getPopShaping(), Configuration::POPSHAPES::PopShaping);
}

TEST(EU4ToVic2_ConfigurationTests, PopShapingFactorDefaultsToFifty)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getPopShapingFactor(), 50.0);
}


TEST(EU4ToVic2_ConfigurationTests, PopShapingFactorCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("popShapingFactor = 50.0");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getPopShapingFactor(), 50.0);
}

TEST(EU4ToVic2_ConfigurationTests, CoreHandlingDefaultsToNone)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getCoreHandling(), Configuration::COREHANDLES::DropNone);
}


TEST(EU4ToVic2_ConfigurationTests, CoreHandlingCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("core_handling = 2");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getCoreHandling(), Configuration::COREHANDLES::DropNational);
}

TEST(EU4ToVic2_ConfigurationTests, DebugDefaultsToFalse)
{
	Configuration testConfiguration;
	std::stringstream input("");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getDebug(), false);
}


TEST(EU4ToVic2_ConfigurationTests, DebugCanBeSet)
{
	Configuration testConfiguration;
	std::stringstream input("debug = yes");
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_EQ(testConfiguration.getDebug(), true);
}


TEST(EU4ToVic2_ConfigurationTests, HybridDefaultsDisabled)
{
	Configuration testConfiguration;
	std::stringstream input;
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_FALSE(testConfiguration.isHpmEnabled());
}


TEST(EU4ToVic2_ConfigurationTests, HybridCanBeEnabled)
{
	Configuration testConfiguration;
	std::stringstream input;
	input << "Vic2directory = \"C:\\Vic2Path\"";
	input << "hybrid_mod = \"2\"";
	testConfiguration.instantiate(input, fakeDoesFolderExist, fakeDoesFileExist);

	ASSERT_TRUE(testConfiguration.isHpmEnabled());
}


