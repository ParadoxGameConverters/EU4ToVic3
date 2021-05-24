#include "EU4ToVic3Converter.h"
#include "Configuration.h"
#include "EU4World/World.h"
#include "Log.h"
#include "V3World/V3World.h"

void convertEU4ToVic3(const std::shared_ptr<mappers::ConverterVersion>& converterVersion)
{
	Log(LogLevel::Progress) << "0 %";
	const auto configuration = std::make_shared<Configuration>(converterVersion);
	Log(LogLevel::Progress) << "4 %";

	Log(LogLevel::Progress) << "5 %";

	const EU4::World sourceWorld(configuration);
	V3::World destWorld(configuration, sourceWorld);

	Log(LogLevel::Info) << "* Conversion complete *";
	Log(LogLevel::Progress) << "100 %";
}
