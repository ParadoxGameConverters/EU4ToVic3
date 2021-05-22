// Helper classes for RGO shuffle.

#ifndef BUCKET_H
#define BUCKET_H

#include <random>
#include <string>
#include <vector>
#include "../../V3World/Province/Province.h"

#include "Parser.h"

namespace mappers
{
	class Bucket : commonItems::parser
	{
	public:
		explicit Bucket(std::istream& theStream);
		
		bool match(const std::string& provClimate, const std::string& provTerrain);
		void shuffle(std::default_random_engine& shuffler);
		void addProvince(std::shared_ptr<V2::Province> prov) { provinces.push_back(prov); };

	private:
		std::string name;
		std::vector<std::string> climates;
		std::vector<std::string> terrains;
		double fraction = 0;
		bool wildClimate = false;
		bool wildTerrain = false;
		std::vector<std::shared_ptr<V2::Province>> provinces;
	};
}

#endif // BUCKET_H
