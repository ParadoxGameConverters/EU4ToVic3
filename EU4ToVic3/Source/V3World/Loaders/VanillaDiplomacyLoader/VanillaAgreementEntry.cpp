#include "VanillaAgreementEntry.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

V3::VanillaAgreementEntry::VanillaAgreementEntry(std::istream& theStream, std::string sourceTag): sourceTag(std::move(sourceTag))
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void V3::VanillaAgreementEntry::registerKeys()
{
	registerKeyword("country", [this](std::istream& theStream) {
		targetTag = commonItems::getString(theStream);
		if ((targetTag.starts_with("c:") || targetTag.starts_with("C:")) && targetTag.size() == 5) // very specific about this.
			targetTag = targetTag.substr(2, 3);
		else
			Log(LogLevel::Warning) << "Unrecognized target truce country: " << targetTag << "!";
	});
	registerKeyword("type", [this](std::istream& theStream) {
		agreementType = commonItems::getString(theStream);
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}
