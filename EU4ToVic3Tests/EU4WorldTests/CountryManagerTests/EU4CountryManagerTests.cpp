#include "CommonFunctions.h"
#include "CountryManager/EU4CountryManager.h"
#include "gtest/gtest.h"
#include <gmock/gmock-matchers.h>
using testing::UnorderedElementsAre;

TEST(EU4World_CountryManagerTests, primitivesDefaultToBlank)
{
	const EU4::CountryManager manager;

	EXPECT_TRUE(manager.getCountries().empty());
}

TEST(EU4World_CountryManagerTests, countriesCanBeLoaded)
{
	std::stringstream input;
	input << "AAA = {}\n";
	input << "BBB = {}\n";
	EU4::CountryManager manager;
	manager.loadCountries(input);

	EXPECT_EQ(2, manager.getCountries().size());
	EXPECT_TRUE(manager.getCountries().contains("AAA"));
	EXPECT_TRUE(manager.getCountries().contains("BBB"));
}

TEST(EU4World_CountryManagerTests, SpecialCountriesAreIgnored)
{
	std::stringstream input;
	input << "--- = {}\n";
	input << "REB = {}\n";
	input << "PIR = {}\n";
	input << "NAT = {}\n";
	input << "AAA = {}\n";
	input << "BBB = {}\n";
	EU4::CountryManager manager;
	manager.loadCountries(input);

	EXPECT_EQ(2, manager.getCountries().size());
	EXPECT_TRUE(manager.getCountries().contains("AAA"));
	EXPECT_TRUE(manager.getCountries().contains("BBB"));
}

TEST(EU4World_CountryManagerTests, CountriesCanBeRetrieved)
{
	std::stringstream input;
	input << "AAA = {}\n";
	input << "BBB = {}\n";
	EU4::CountryManager manager;
	manager.loadCountries(input);

	EXPECT_EQ("AAA", manager.getCountry("AAA")->getTag());
	EXPECT_EQ("BBB", manager.getCountry("BBB")->getTag());
}

TEST(EU4World_CountryManagerTests, CountryMismatchReturnsNullptr)
{
	std::stringstream input;
	EU4::CountryManager manager;
	manager.loadCountries(input);

	EXPECT_EQ(nullptr, manager.getCountry("AAA"));
}

TEST(EU4World_CountryManagerTests, LinkingProvincesToOwnersWorks)
{
	std::stringstream provincesInput;
	provincesInput << "-1 = { owner=TAG }\n";
	provincesInput << "-2 = { owner=TAG }\n";
	provincesInput << "-3 = { owner=GAT }\n";
	provincesInput << "-4 = { }\n";				// unowned
	provincesInput << "-5 = { owner=BLA }\n"; // nonsense
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provincesInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "TAG = { capital = 2 }\n";
	countryManagerInput << "GAT = { capital = 3 }\n";
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);

	manager.linkProvincesToCountries(provinceManager);

	const auto& tag = manager.getCountry("TAG");
	const auto& gat = manager.getCountry("GAT");

	EXPECT_EQ(2, tag->getProvinces().size());
	EXPECT_EQ(1, tag->getProvinces().at(1)->getID());
	EXPECT_TRUE(tag->getProvinces().at(1)->getCapitals().empty());
	EXPECT_EQ(2, tag->getProvinces().at(2)->getID());
	EXPECT_THAT(tag->getProvinces().at(2)->getCapitals(), testing::UnorderedElementsAre("TAG"));

	EXPECT_EQ(1, gat->getProvinces().size());
	EXPECT_EQ(3, gat->getProvinces().at(3)->getID());
	EXPECT_THAT(gat->getProvinces().at(3)->getCapitals(), testing::UnorderedElementsAre("GAT"));
}

TEST(EU4World_CountryManagerTests, LinkingProvincesToCoreOwnersWorks)
{
	std::stringstream provincesInput;
	provincesInput << "-1 = { cores = { TAG GAT } }\n";
	provincesInput << "-2 = { cores = { TAG } }\n";
	provincesInput << "-3 = { cores = { GAT } }\n";
	provincesInput << "-4 = { }\n";						// no cores
	provincesInput << "-5 = { cores = { BLA } }\n"; // nonsense
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provincesInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "TAG = {}\n";
	countryManagerInput << "GAT = {}\n";
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);

	manager.linkProvincesToCountries(provinceManager);

	const auto& tag = manager.getCountry("TAG");
	const auto& gat = manager.getCountry("GAT");

	EXPECT_EQ(2, tag->getCores().size());
	EXPECT_EQ(1, tag->getCores().at(1)->getID());
	EXPECT_EQ(2, tag->getCores().at(2)->getID());

	EXPECT_EQ(2, gat->getCores().size());
	EXPECT_EQ(1, gat->getCores().at(1)->getID());
	EXPECT_EQ(3, gat->getCores().at(3)->getID());
}

TEST(EU4World_CountryManagerTests, HREStatusInheritedFromCapitalProvince)
{
	std::stringstream provincesInput;
	provincesInput << "-1 = { hre = yes } }\n";
	provincesInput << "-2 = { }\n";
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provincesInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "TAG = { capital = 1 }\n";
	countryManagerInput << "GAT = { capital = 2 }\n";
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);

	manager.setHREAndEmperors("", "", provinceManager);

	const auto& tag = manager.getCountry("TAG");
	const auto& gat = manager.getCountry("GAT");

	EXPECT_TRUE(tag->isInHRE());
	EXPECT_FALSE(gat->isInHRE());
}

TEST(EU4World_CountryManagerTests, EmperorshipsAreForwardedToCountries)
{
	const EU4::ProvinceManager provinceManager;

	std::stringstream countryManagerInput;
	countryManagerInput << "TAG = { }\n";
	countryManagerInput << "GAT = { }\n";
	countryManagerInput << "AAA = { }\n";
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);

	manager.setHREAndEmperors("TAG", "GAT", provinceManager);

	const auto& tag = manager.getCountry("TAG");
	const auto& gat = manager.getCountry("GAT");
	const auto& aaa = manager.getCountry("AAA");

	EXPECT_TRUE(tag->isHREmperor());
	EXPECT_FALSE(tag->isCelestialEmperor());
	EXPECT_FALSE(gat->isHREmperor());
	EXPECT_TRUE(gat->isCelestialEmperor());
	EXPECT_FALSE(aaa->isHREmperor());
	EXPECT_FALSE(aaa->isCelestialEmperor());
}

TEST(EU4World_CountryManagerTests, RevolutionaryIsForwardedToCountries)
{
	std::stringstream countryManagerInput;
	countryManagerInput << "TAG = { }\n";
	countryManagerInput << "GAT = { }\n";
	countryManagerInput << "AAA = { }\n";
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);

	manager.setRevolutionTarget("GAT");

	const auto& tag = manager.getCountry("TAG");
	const auto& gat = manager.getCountry("GAT");
	const auto& aaa = manager.getCountry("AAA");

	EXPECT_FALSE(tag->isRevolutionary());
	EXPECT_TRUE(gat->isRevolutionary());
	EXPECT_FALSE(aaa->isRevolutionary());
}

TEST(EU4World_CountryManagerTests, JapanCanBeUnited)
{
	std::stringstream provincesInput;
	provincesInput << "-1 = { owner = AAA cores={ AAA } }\n";
	provincesInput << "-2 = { owner = BBB cores={ BBB } }\n";
	provincesInput << "-3 = { owner = CCC cores={ CCC } }\n";
	provincesInput << "-4 = { owner = DDD cores={ DDD } }\n";
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provincesInput);

	std::stringstream nationMergeInput;
	nationMergeInput << "merge_daimyos = yes";
	mappers::NationMergeMapper mapper;
	mapper.loadNationMerge(nationMergeInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "AAA = { government = { reform_stack={ reforms={ daimyo } } } }\n";
	countryManagerInput << "BBB = { government = { reform_stack={ reforms={ indep_daimyo } } } }\n";
	countryManagerInput << "CCC = { government = { reform_stack={ reforms={ shogunate } } } }\n";
	countryManagerInput << "DDD = { government = { reform_stack={ reforms={ plutocracy } } } }\n"; // control group.
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);
	manager.linkProvincesToCountries(provinceManager);
	manager.loadNationMergeMapper(mapper);
	manager.mergeNations();

	EXPECT_THAT(provinceManager.getProvince(1)->getCores(), UnorderedElementsAre("CCC"));
	EXPECT_THAT(provinceManager.getProvince(2)->getCores(), UnorderedElementsAre("CCC"));
	EXPECT_THAT(provinceManager.getProvince(3)->getCores(), UnorderedElementsAre("CCC"));
	EXPECT_THAT(provinceManager.getProvince(4)->getCores(), UnorderedElementsAre("DDD"));
	EXPECT_EQ("CCC", provinceManager.getProvince(1)->getOwnerTag());
	EXPECT_EQ("CCC", provinceManager.getProvince(2)->getOwnerTag());
	EXPECT_EQ("CCC", provinceManager.getProvince(3)->getOwnerTag());
	EXPECT_EQ("DDD", provinceManager.getProvince(4)->getOwnerTag());
}

TEST(EU4World_CountryManagerTests, NationsCanBeMerged)
{
	std::stringstream provincesInput;
	provincesInput << "-1 = { owner = AAA cores={ AAA } }\n";
	provincesInput << "-2 = { owner = BBB cores={ BBB } }\n";
	provincesInput << "-3 = { owner = CCC cores={ CCC } }\n";
	provincesInput << "-4 = { owner = DDD cores={ DDD } }\n";
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provincesInput);

	std::stringstream nationMergeInput;
	nationMergeInput << "test = {\n";
	nationMergeInput << " merge = yes\n";
	nationMergeInput << " master = BBB\n";
	nationMergeInput << " slave = AAA\n";
	nationMergeInput << " slave = DDD\n";
	nationMergeInput << "}\n";
	mappers::NationMergeMapper mapper;
	mapper.loadNationMerge(nationMergeInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "AAA = { }\n";
	countryManagerInput << "BBB = { }\n";
	countryManagerInput << "CCC = { }\n"; // control group.
	countryManagerInput << "DDD = { }\n";
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);
	manager.linkProvincesToCountries(provinceManager);
	manager.loadNationMergeMapper(mapper);
	manager.mergeNations();

	EXPECT_THAT(provinceManager.getProvince(1)->getCores(), UnorderedElementsAre("BBB"));
	EXPECT_THAT(provinceManager.getProvince(2)->getCores(), UnorderedElementsAre("BBB"));
	EXPECT_THAT(provinceManager.getProvince(3)->getCores(), UnorderedElementsAre("CCC"));
	EXPECT_THAT(provinceManager.getProvince(4)->getCores(), UnorderedElementsAre("BBB"));
	EXPECT_EQ("BBB", provinceManager.getProvince(1)->getOwnerTag());
	EXPECT_EQ("BBB", provinceManager.getProvince(2)->getOwnerTag());
	EXPECT_EQ("CCC", provinceManager.getProvince(3)->getOwnerTag());
	EXPECT_EQ("BBB", provinceManager.getProvince(4)->getOwnerTag());
}

TEST(EU4World_CountryManagerTests, LocalizationsCanBeInjected)
{
	std::stringstream input;
	input << commonItems::utf8BOM << "l_english:\n";
	input << " AAA: \"aloc\" # comment\n";
	input << " BBB: \"bloc\"\n";
	input << " AAA_ADJ: \"aloc adj\" # comment\n";
	input << " BBB_ADJ: \"bloc adj\"\n";
	std::stringstream input2;
	input2 << commonItems::utf8BOM << "l_french:\n";
	input2 << " AAA: \"alocee\"\n";
	input2 << " BBB: \"blocee\"\n";
	input2 << " AAA_ADJ: \"alocee adj\"\n";
	input2 << " BBB_ADJ: \"blocee adj\"\n";
	EU4::EU4LocalizationLoader locs;
	locs.loadLocalizations(input);
	locs.loadLocalizations(input2);

	std::stringstream countryManagerInput;
	countryManagerInput << "AAA = { name = \"le default a\" adjective = \"le default a adj\" }\n";
	countryManagerInput << "BBB = { name = \"le default b\" adjective = \"le default b adj\" }\n";
	countryManagerInput << "CCC = { name = \"le default c\" adjective = \"le default c adj\" }\n"; // control group.
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);
	manager.loadLocalizations(locs);
	manager.injectLocalizations();

	EXPECT_EQ("aloc", manager.getCountry("AAA")->getName("english"));
	EXPECT_EQ("alocee", manager.getCountry("AAA")->getName("french"));
	EXPECT_EQ("aloc", manager.getCountry("AAA")->getName("nonsense"));
	EXPECT_EQ("aloc adj", manager.getCountry("AAA")->getAdjective("english"));
	EXPECT_EQ("alocee adj", manager.getCountry("AAA")->getAdjective("french"));
	EXPECT_EQ("aloc adj", manager.getCountry("AAA")->getAdjective("nonsense"));

	EXPECT_EQ("bloc", manager.getCountry("BBB")->getName("english"));
	EXPECT_EQ("blocee", manager.getCountry("BBB")->getName("french"));
	EXPECT_EQ("bloc", manager.getCountry("BBB")->getName("nonsense"));
	EXPECT_EQ("bloc adj", manager.getCountry("BBB")->getAdjective("english"));
	EXPECT_EQ("blocee adj", manager.getCountry("BBB")->getAdjective("french"));
	EXPECT_EQ("bloc adj", manager.getCountry("BBB")->getAdjective("nonsense"));

	EXPECT_EQ("le default c", manager.getCountry("CCC")->getName("english"));
	EXPECT_EQ("le default c", manager.getCountry("CCC")->getName("french"));
	EXPECT_EQ("le default c", manager.getCountry("CCC")->getName("nonsense"));
	EXPECT_EQ("le default c adj", manager.getCountry("CCC")->getAdjective("english"));
	EXPECT_EQ("le default c adj", manager.getCountry("CCC")->getAdjective("french"));
	EXPECT_EQ("le default c adj", manager.getCountry("CCC")->getAdjective("nonsense"));
}

TEST(EU4World_CountryManagerTests, EmptyNationsAreDeleted)
{
	std::stringstream provincesInput;
	provincesInput << "-1 = { owner = AAA cores={ AAA } }\n";
	provincesInput << "-2 = { owner = AAA cores={ BBB } }\n";
	provincesInput << "-3 = { owner = CCC cores={ CCC } }\n";
	provincesInput << "-4 = { owner = AAA cores={ BBB } }\n";
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provincesInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "AAA = { }\n";
	countryManagerInput << "BBB = { }\n";
	countryManagerInput << "CCC = { }\n";
	countryManagerInput << "DDD = { }\n"; // no provinces, no cores, to be deleted.
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);
	manager.linkProvincesToCountries(provinceManager);

	manager.filterDeadNations(Configuration::DEADCORES::LeaveAll);

	EXPECT_TRUE(manager.getCountry("AAA"));
	EXPECT_TRUE(manager.getCountry("BBB"));
	EXPECT_TRUE(manager.getCountry("CCC"));
	EXPECT_FALSE(manager.getCountry("DDD"));
}

TEST(EU4World_CountryManagerTests, ProvinceLessNationsCanBeDeleted)
{
	std::stringstream provincesInput;
	provincesInput << "-1 = { owner = AAA cores={ AAA } }\n";
	provincesInput << "-2 = { owner = AAA cores={ BBB } }\n";
	provincesInput << "-3 = { owner = CCC cores={ CCC } }\n";
	provincesInput << "-4 = { owner = AAA cores={ BBB } }\n";
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provincesInput);

	std::stringstream countryManagerInput;
	countryManagerInput << "AAA = { }\n";
	countryManagerInput << "BBB = { }\n"; // no provinces, to be deleted.
	countryManagerInput << "CCC = { }\n";
	countryManagerInput << "DDD = { }\n"; // no provinces, no cores, deleted automatically.
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);
	manager.linkProvincesToCountries(provinceManager);

	manager.filterDeadNations(Configuration::DEADCORES::AllCores);

	EXPECT_TRUE(manager.getCountry("AAA"));
	EXPECT_FALSE(manager.getCountry("BBB"));
	EXPECT_TRUE(manager.getCountry("CCC"));
	EXPECT_FALSE(manager.getCountry("DDD"));
}

TEST(EU4World_CountryManagerTests, CultureLessNationsCanBeDeleted)
{
	std::stringstream provincesInput;
	provincesInput << "-1 = { owner = AAA cores={ AAA } culture = livingCulture religion = irrelevant }\n";
	provincesInput << "-2 = { owner = AAA cores={ BBB } culture = livingCulture religion = irrelevant }\n";
	provincesInput << "-3 = { owner = AAA cores={ CCC } culture = livingCulture religion = irrelevant }\n";
	provincesInput << "-4 = { owner = AAA cores={ BBB } culture = livingCulture religion = irrelevant }\n";
	EU4::ProvinceManager provinceManager;
	provinceManager.loadProvinces(provincesInput);

	DatingData datingData;
	datingData.startEU4Date = date("1444.11.11");
	provinceManager.buildPopRatios(datingData, false);

	std::stringstream countryManagerInput;
	countryManagerInput << "AAA = { primary_culture=foreignCulture }\n";
	countryManagerInput << "BBB = { primary_culture=livingCulture }\n"; // culture survives in province 2, country survives.
	countryManagerInput << "CCC = { primary_culture=deadCulture }\n";	  // culture does not survive in province 3, country is deleted.
	countryManagerInput << "DDD = { primary_culture=deadCulture }\n";	  // no provinces, no cores, deleted automatically.
	EU4::CountryManager manager;
	manager.loadCountries(countryManagerInput);
	manager.linkProvincesToCountries(provinceManager);

	manager.filterDeadNations(Configuration::DEADCORES::DeadCores);

	EXPECT_TRUE(manager.getCountry("AAA"));
	EXPECT_TRUE(manager.getCountry("BBB"));
	EXPECT_FALSE(manager.getCountry("CCC"));
	EXPECT_FALSE(manager.getCountry("DDD"));
}
