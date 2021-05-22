#ifndef REGIMENT_H
#define REGIMENT_H

#include <map>
#include <string>

namespace V2
{
	enum class REGIMENTTYPE
	{
		// ground units
		irregular,
		infantry,
		cavalry,
		artillery,
		// navy units
		manowar,
		frigate,
		clipper_transport,
	};

	static std::map<std::string, REGIMENTTYPE> RegimentNameToType
	{
		{ "irregular", REGIMENTTYPE::irregular },
		{ "infantry", REGIMENTTYPE::infantry },
		{ "cavalry", REGIMENTTYPE::cavalry },
		{ "artillery", REGIMENTTYPE::artillery },
		{ "manowar", REGIMENTTYPE::manowar},
		{ "frigate", REGIMENTTYPE::frigate },
		{ "clipper_transport", REGIMENTTYPE::clipper_transport }
	};

	static std::map<REGIMENTTYPE, std::string> RegimentTypeToName
	{
		{ REGIMENTTYPE::irregular, "irregular" },
		{ REGIMENTTYPE::infantry, "infantry" },
		{ REGIMENTTYPE::cavalry, "cavalry" },
		{ REGIMENTTYPE::artillery, "artillery" },
		{ REGIMENTTYPE::manowar, "manowar" },
		{ REGIMENTTYPE::frigate, "frigate" },
		{ REGIMENTTYPE::clipper_transport, "clipper_transport" }
	};

	class Regiment // also Ship
	{
	public:
		explicit Regiment(REGIMENTTYPE regimentType);

		void setName(const std::string& _name) { name = _name; }
		void setHome(const int newHome) { homeProvinceID = newHome; }

		[[nodiscard]] auto getShip() const { return isShip; }
		[[nodiscard]] auto getType()	const { return regimentType; }

		friend std::ostream& operator<<(std::ostream& output, const Regiment& regiment);

	private:
		std::string name;
		int homeProvinceID = 0;
		bool isShip = false;
		REGIMENTTYPE regimentType;
	};
}

#endif // REGIMENT_H
