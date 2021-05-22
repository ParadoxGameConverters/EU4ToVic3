#ifndef MOD_FILE_H
#define MOD_FILE_H

#include <ostream>

namespace V2
{	
	class ModFile
	{
	public:		
		friend std::ostream& operator<<(std::ostream& output, const ModFile& modFile);
	};
}
#endif // MOD_FILE_H
