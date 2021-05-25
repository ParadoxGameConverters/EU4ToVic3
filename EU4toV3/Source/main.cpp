#include "EU4ToVic3Converter.h"
#include "Log.h"
#include "ConverterVersion/ConverterVersion.h"
#include <fstream>

int main(const int argc, const char* argv[])
{
	try
	{
		std::ofstream clearLog("log.txt");
		clearLog.close();
		
		mappers::ConverterVersion converterVersion;
		converterVersion.loadVersion("configurables/version.txt");
		
		Log(LogLevel::Info) << converterVersion;
		if (argc >= 2)
		{
			Log(LogLevel::Info) << "EU4toVic3 takes no parameters.";
			Log(LogLevel::Info) << "It uses configuration.txt, configured manually or by the frontend.";
		}
		convertEU4ToVic3(std::move(converterVersion));
		return 0;
	}

	catch (const std::exception& e)
	{
		Log(LogLevel::Error) << e.what();
		return -1;
	}
}