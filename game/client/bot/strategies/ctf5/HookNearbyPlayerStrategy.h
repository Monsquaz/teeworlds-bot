#ifndef HOOKNEARBYPLAYERSTRATEGY_H
#define HOOKNEARBYPLAYERSTRATEGY_H

#include "../BotStrategy.h"

class HookNearbyPlayerStrategy : public BotStrategy {
public:

	HookNearbyPlayerStrategy(CGameClient* client);

	void execute();

private:

	void attack(vec2* targetPos);
	char *getRandomLine();
	void go(bool right, bool withHooking, float yPos);
	int spotIndex;
	int maxYPos;
	long chatTime;
	long hookTime;
	long attackTime;
	long spotTime;
	char chatMessage[200];

};

#endif /* HOOKNEARBYPLAYERSTRATEGY_H */
