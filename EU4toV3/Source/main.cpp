#include "EU4ToVic3Converter.h"
#include "Log.h"
#include "Mappers/VersionParser/VersionParser.h"
#include <fstream>

int main(const int argc, const char* argv[])
{
	try
	{
		std::ofstream clearLog("log.txt");
		clearLog.close();
		const mappers::VersionParser versionParser;
		LOG(LogLevel::Info) << versionParser;
		if (argc >= 2)
		{
			LOG(LogLevel::Info) << "EU4toVic3 takes no parameters.";
			LOG(LogLevel::Info) << "It uses configuration.txt, configured manually or by the frontend.";
		}
		convertEU4ToVic3(versionParser);
		return 0;
	}

	catch (const std::exception& e)
	{
		LOG(LogLevel::Error) << e.what();
		return -1;
	}
}