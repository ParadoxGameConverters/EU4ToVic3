#include "StateModifierLoader.h"
#include "ClayManager/State/StateModifier.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"

void V3::StateModifierLoader::loadStateModifiers(const commonItems::ModFilesystem& modFS)
{
	registerKeys();
	for (const auto& fileName: modFS.GetAllFilesInFolder("/common/state_traits/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(fileName);
	}
	clearRegisteredKeywords();
}

void V3::StateModifierLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& modifierName, std::istream& theStream) {
		StateModifier newModifier;
		newModifier.loadStateModifier(theStream);
		newModifier.setName(modifierName);
		stateModifiers.emplace(modifierName, newModifier);
	});
}
