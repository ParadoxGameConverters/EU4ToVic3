#include "Leader.h"

V2::Leader::Leader(const EU4::Leader& oldLeader, const mappers::LeaderTraitMapper& leaderTraitMapper):
	name(oldLeader.getName()),
	activationDate(oldLeader.getActivationDate()),
	isLand(oldLeader.isLand())
{
	const auto& incPersonality = leaderTraitMapper.getPersonality(oldLeader.getFire(), oldLeader.getShock(), oldLeader.getManeuver(), oldLeader.getSiege());
	const auto& incBackground = leaderTraitMapper.getBackground(oldLeader.getFire(), oldLeader.getShock(), oldLeader.getManeuver(), oldLeader.getSiege());
	if (incPersonality) personality = *incPersonality;
	if (incBackground) background = *incBackground;
}
