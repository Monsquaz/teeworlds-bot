#ifndef ALWAYSVOTENOSTRATEGY_H
#define ALWAYSVOTENOSTRATEGY_H

#include "../BotStrategy.h"

class AlwaysVoteNoStrategy : public BotStrategy {
public:

	AlwaysVoteNoStrategy(CGameClient* client);

	void execute();

private:

};

#endif /* ALWAYSVOTENOSTRATEGY_H */
