#include "StateModifierLoader.h"
#include "ClayManager/State/StateModifier.h"
#include "CommonFunctions.h"
#include "CommonRegexes.h"
#include "OSCompatibilityLayer.h"

void V3::StateModifierLoader::loadStateModifiers(const std::string& v3Path)
{
	registerKeys();
	for (const auto& fileName: commonItems::GetAllFilesInFolder(v3Path + "/common/state_traits/"))
	{
		if (getExtension(fileName) == "txt")
			parseFile(v3Path + "/common/state_traits/" + fileName);
	}
	clearRegisteredKeywords();
}

void V3::StateModifierLoader::registerKeys()
{
	registerRegex(commonItems::catchallRegex, [this](const std::string& modifierName, std::istream& theStream) {
		const auto newModifier = std::make_shared<StateModifier>();
		newModifier->loadStateModifier(theStream);
		newModifier->setName(modifierName);
		stateModifiers.emplace(modifierName, newModifier);
	});
}
