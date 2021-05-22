#ifndef LOCALISATION_H
#define LOCALISATION_H

#include "../../EU4World/Country/EU4Country.h"

namespace V2
{
	// Holds translations for attributes of a specific V2 country.
	class Localisation
	{
	public:
		// Sets the tag to use for creating the name and adjective key to use in the localization output.
		void setTag(const std::string& tag);
		// Populates the localized names and adjectives using information from the EU4 country.
		void readFromCountry(const EU4::Country&);

		// Sets the key to use for the specified party in the localization output.
		void setPartyKey(size_t partyIndex, const std::string& partyKey);
		// Sets the localized party name for the specified party in the given language, e.g. "english".
		void setPartyName(size_t partyIndex, const std::string& language, const std::string& name);

		// Writes a V2-formatted localization info for all localized elements as:
		// key;translation0;translation1;...;;;x
		friend std::ostream& operator<<(std::ostream& output, const Localisation& localisation);

		// All localizations are stored internally as UTF-8. However V2 doesn't seem to handle Unicode
		// so we convert the text to the appropriate Windows codepage (1250, 1251, and 1252).
		static std::string convert(const std::string&, int language = 0);
		static std::string stripAccents(const std::string&);

		[[nodiscard]] std::string getLocalName() const;
		[[nodiscard]] std::string getLocalAdjective() const;

	private:
		static const size_t numLanguages = 14;
		static const std::array<std::string, numLanguages> languages;

		typedef std::array<std::string, numLanguages> Localisations;

		std::string tag;
		Localisations name;
		Localisations adjective;

		struct Party
		{
			std::string key;
			Localisations name;
		};
		std::vector<Party> parties;
	};
}

#endif // LOCALISATION_H
