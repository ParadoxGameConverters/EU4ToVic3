#include "War.h"
#include "Log.h"

bool V2::War::loadWar(const EU4::War& eu4War,
	const mappers::WarGoalMapper& warGoalMapper,
	const mappers::ProvinceMapper& provinceMapper,
	const mappers::CountryMappings& countryMapper,
	std::map<std::string, std::shared_ptr<Country>>& countries)
{
	details = eu4War.getDetails();
	name = eu4War.getName();

	std::vector<std::string> translatedAttackers; // vector to preserve order. First attacker is primary.
	std::vector<std::string> translatedDefenders; // vector to preserve order. First attacker is primary.
	for (const auto& actor : eu4War.getAttackers())
	{
		// The preambule to make sure we're not fighting ghosts.
		const auto& V2Tag = translateActor(actor, countryMapper, countries);
		if (!V2Tag) continue;
		translatedAttackers.push_back(*V2Tag);
	}
	for (const auto& actor : eu4War.getDefenders())
	{
		// The preambule to make sure we're not fighting ghosts.
		const auto& V2Tag = translateActor(actor, countryMapper, countries);
		if (!V2Tag) continue;
		translatedDefenders.push_back(*V2Tag);
	}

	attackers.swap(translatedAttackers);
	defenders.swap(translatedDefenders);

	const auto& ifWarGoal = warGoalMapper.translateWarGoal(details.warGoalType);
	if (ifWarGoal)
	{
		details.warGoalType = *ifWarGoal;
	}
	else
	{
		Log(LogLevel::Warning) << "Unable to translate war CB: " << details.warGoalType << ", skipping war " << name << "!";
		return false;
	}

	// A good, solid war needs two parties and a CB. Rest are details.	
	if (attackers.empty() || defenders.empty() || details.warGoalType.empty()) return false;

	const auto& ifTargetTag = translateActor(details.targetTag, countryMapper, countries);
	if (ifTargetTag)
	{
		details.targetTag = *ifTargetTag;
	}
	else
	{
		details.targetTag.clear();
	}

	// we should map the target province to something, or reset it to zero if we cannot.
	const auto& ifProvinceID = provinceMapper.getVic2ProvinceNumbers(details.targetProvinceID);
	!ifProvinceID.empty() ? details.targetProvinceID = *ifProvinceID.begin() : details.targetProvinceID = 0;
	return true;
}

std::optional<std::string> V2::War::translateActor(
	const std::string & actor,
	const mappers::CountryMappings & countryMapper,
	std::map<std::string, std::shared_ptr<Country>> & countries)
{
	// We need a war party that is not dead, ie. haven't died during province remap.
	auto ifV2Tag = countryMapper.getV2Tag(actor);
	if (!ifV2Tag) return std::nullopt;
	auto V2Tag = *ifV2Tag;
	const auto& country = countries.find(V2Tag);
	if (country == countries.end()) return std::nullopt;
	if (country->second->getProvinces().empty()) return std::nullopt;
	return V2Tag;
}

std::string V2::War::generateFileName() const
{
	std::string newName;
	if (!attackers.empty()) newName += attackers[0];
	newName += "_vs_";
	if (!defenders.empty()) newName += defenders[0];
	newName += "_ImportedWar_";
	newName += details.warGoalClass;
	newName += ".txt";
	return newName;
}