#include "ProvinceManager.h"
#include "CommonRegexes.h"
#include "Log.h"
#include "ParserHelpers.h"

void EU4::ProvinceManager::loadProvinces(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ProvinceManager::registerKeys()
{
	registerRegex(commonItems::integerRegex, [this](const std::string& numberString, std::istream& theStream) {
		auto newProvince = std::make_shared<Province>(numberString, theStream);
		provinces.emplace(newProvince->getID(), std::move(newProvince));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

const std::shared_ptr<EU4::Province>& EU4::ProvinceManager::getProvince(const int provinceID) const
{
	if (!provinces.contains(provinceID))
		throw std::runtime_error(
			 std::string("Accessing EU4 province ") + std::to_string(provinceID) + std::string(" that does not exist (Save/EU4 version mismatch?)"));
	else
		return provinces.at(provinceID);
}
