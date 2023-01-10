#include "Loaders/VanillaDiplomacyLoader/VanillaAgreementEntry.h"
#include "gtest/gtest.h"

TEST(V3World_VanillaAgreementEntryTests, DefaultsDefaultToDefaults)
{
	const V3::VanillaAgreementEntry entry;

	EXPECT_TRUE(entry.getSourceTag().empty());
	EXPECT_TRUE(entry.getTargetTag().empty());
	EXPECT_TRUE(entry.getAgreementType().empty());
}

TEST(V3World_VanillaAgreementEntryTests, entryCanBeLoaded)
{
	std::stringstream input;
	input << "country = c:POR type = defensive_pact\n";
	const V3::VanillaAgreementEntry entry(input, "GBR");

	EXPECT_EQ("GBR", entry.getSourceTag());
	EXPECT_EQ("POR", entry.getTargetTag());
	EXPECT_EQ("defensive_pact", entry.getAgreementType());
}
