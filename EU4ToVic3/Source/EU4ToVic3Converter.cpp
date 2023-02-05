#include "EU4ToVic3Converter.h"
#include "Configuration.h"
#include "EU4World/World.h"
#include "Log.h"
#include "Output/outWorld.h"
#include "V3World/V3World.h"

void convertEU4ToVic3(commonItems::ConverterVersion&& converterVersion)
{
	Log(LogLevel::Progress) << "0 %";
	auto configuration = std::make_shared<Configuration>(converterVersion);
	Log(LogLevel::Info) << "<> Configuration imported.";
	Log(LogLevel::Progress) << "4 %";

	const EU4::World sourceWorld(configuration, converterVersion);
	const V3::World destWorld(*configuration, sourceWorld);
	OUT::exportWorld(*configuration, destWorld, converterVersion);

	Log(LogLevel::Notice) << "* Conversion complete *";
	Log(LogLevel::Progress) << "100 %";
}
