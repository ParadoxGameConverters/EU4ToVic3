#include "ProvinceManager.h"
#include "Log.h"
#include "ParserHelpers.h"
#include "CommonRegexes.h"

EU4::ProvinceManager::ProvinceManager(std::istream& theStream)
{
	registerKeys();
	parseStream(theStream);
	clearRegisteredKeywords();
}

void EU4::ProvinceManager::registerKeys()
{
	registerRegex(commonItems::integerRegex, [this](const std::string& numberString, std::istream& theStream) {
		auto newProvince = std::make_shared<Province>(numberString, theStream);
		provinces.insert(std::make_pair(newProvince->getNum(), std::move(newProvince)));
	});
	registerRegex(commonItems::catchallRegex, commonItems::ignoreItem);
}

const std::shared_ptr<EU4::Province>& EU4::ProvinceManager::getProvince(const int provinceNumber) const
{
	if (!provinces.contains(provinceNumber))
		throw std::range_error(std::string("Accessing EU4 province ") + std::to_string(provinceNumber) +
									  std::string(" that does not exist (Area mismatch?)"));
	else
		return provinces.at(provinceNumber);
}
