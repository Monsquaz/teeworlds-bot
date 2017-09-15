#include "AlwaysVoteNoStrategy.h"
#include <game/client/components/voting.h>

AlwaysVoteNoStrategy::AlwaysVoteNoStrategy(CGameClient* client) : BotStrategy(client) {
}

void AlwaysVoteNoStrategy::execute() {
	if(client->m_pVoting->IsVoting() && !client->m_pVoting->TakenChoice()) {
		client->m_pVoting->Vote(-1); // Vote no!
	}
}
