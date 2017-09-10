#ifndef HOOKNEARBYPLAYERSTRATEGY_H
#define HOOKNEARBYPLAYERSTRATEGY_H

#include "../BotStrategy.h"

class HookNearbyPlayerStrategy : public BotStrategy {
public:

	HookNearbyPlayerStrategy(CGameClient* client);

	void execute();

private:
	
	void attack(vec2* targetPos);

};

#endif /* HOOKNEARBYPLAYERSTRATEGY_H */

