#include "EU4ToVic3Converter.h"
#include "Configuration.h"
#include "EU4World/World.h"
#include "Log.h"
#include "V3World/V3World.h"

void convertEU4ToVic3(mappers::ConverterVersion&& converterVersion)
{
	Log(LogLevel::Progress) << "0 %";
	const auto configuration = Configuration();
	Log(LogLevel::Progress) << "4 %";

	Log(LogLevel::Progress) << "5 %";

	const EU4::World sourceWorld(configuration, converterVersion);
	V3::World destWorld(configuration, sourceWorld, std::move(converterVersion));

	Log(LogLevel::Info) << "* Conversion complete *";
	Log(LogLevel::Progress) << "100 %";
}
