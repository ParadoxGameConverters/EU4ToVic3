#include "Configuration.h"
#include "Log.h"
#include "OSCompatibilityLayer.h"
#include "EU4World/World.h"
#include "Mappers/IdeaEffects/IdeaEffectMapper.h"
#include "Mappers/TechGroups/TechGroupsMapper.h"
#include "V3World/V3World.h"
#include "Mappers/VersionParser/VersionParser.h"
#include "EU4ToVic3Converter.h"

void convertEU4ToVic2(const mappers::VersionParser& versionParser)
{
	Log(LogLevel::Progress) << "0 %";
	ConfigurationFile configurationFile("configuration.txt");
	deleteExistingOutputFolder();
	Log(LogLevel::Progress) << "4 %";

	const mappers::IdeaEffectMapper ideaEffectMapper;
	const mappers::TechGroupsMapper techGroupsMapper;
	Log(LogLevel::Progress) << "5 %";

	const EU4::World sourceWorld(ideaEffectMapper);
	V2::World destWorld(sourceWorld, ideaEffectMapper, techGroupsMapper, versionParser);

	LOG(LogLevel::Info) << "* Conversion complete *";
	Log(LogLevel::Progress) << "100 %";
}


void deleteExistingOutputFolder()
{
	const auto outputFolder = "output/" + theConfiguration.getOutputName();
	if (commonItems::DoesFolderExist(outputFolder))
	{
		if (!commonItems::DeleteFolder(outputFolder))
		{
			throw std::runtime_error("Could not delete pre-existing output folder: output/" + theConfiguration.getOutputName());
		}
	}
}