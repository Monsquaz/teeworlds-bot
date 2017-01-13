#include "Step4_JumpUpToTheChamberStrategy.h"

#include "game/client/bot/BotUtil.h"

Step4_JumpUpToTheChamberStrategy::Step4_JumpUpToTheChamberStrategy(CGameClient* client) :
BotStrategy(client) {
}

const vec2 Step4_JumpUpToTheChamberStrategy::TARGET_POS = vec2(2129, 657);

void Step4_JumpUpToTheChamberStrategy::execute(CControls* controls) {
	CCharacterCore* player = &client->m_PredictedChar;
	vec2 pos = player->m_Pos;

	if (pos.x == TARGET_POS.x && pos.y == TARGET_POS.y) {
		controls->m_MousePos.x = -260;
		controls->m_MousePos.y = -240;
		controls->m_InputData.m_Hook = 1;
	} else if (player->m_HookState != HOOK_GRABBED) {
		BotUtil::moveTowards(controls, pos.x, TARGET_POS.x);
		controls->m_InputData.m_Jump = pos.y > TARGET_POS.y && player->IsGrounded();
	}
}