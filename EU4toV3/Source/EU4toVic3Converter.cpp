#include "EU4ToVic3Converter.h"
#include "Configuration.h"
#include "EU4World/World.h"
#include "Log.h"
#include "Mappers/IdeaEffects/IdeaEffectMapper.h"
#include "Mappers/TechGroups/TechGroupsMapper.h"
#include "Mappers/VersionParser/VersionParser.h"
#include "V3World/V3World.h"

void convertEU4ToVic3(const mappers::VersionParser& versionParser)
{
	Log(LogLevel::Progress) << "0 %";
	const auto theConfiguration = std::make_shared<Configuration>();
	deleteExistingOutputFolder();
	Log(LogLevel::Progress) << "4 %";

	const mappers::IdeaEffectMapper ideaEffectMapper;
	const mappers::TechGroupsMapper techGroupsMapper;
	Log(LogLevel::Progress) << "5 %";

	const EU4::World sourceWorld(theConfiguration, ideaEffectMapper);
	V2::World destWorld(theConfiguration, sourceWorld, ideaEffectMapper, techGroupsMapper, versionParser);

	Log(LogLevel::Info) << "* Conversion complete *";
	Log(LogLevel::Progress) << "100 %";
}
