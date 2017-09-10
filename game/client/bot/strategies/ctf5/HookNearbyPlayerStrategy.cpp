#include "HookNearbyPlayerStrategy.h"

HookNearbyPlayerStrategy::HookNearbyPlayerStrategy(CGameClient* client) :
BotStrategy(client) {
}

void HookNearbyPlayerStrategy::execute() {
    CCharacterCore* me = &client->m_PredictedChar;
    int myId = client->m_Snap.m_LocalClientID;

    int closestPlayerId = -1;
    float closestPlayerDistance = 999999;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (i == myId || !client->m_Snap.m_aCharacters[i].m_Active)
            continue;

        CCharacterCore* otherPlayer = &client->m_aClients[i].m_Predicted;
        float distanceToOtherPlayer = distance(me->m_Pos, otherPlayer->m_Pos);
        if (distanceToOtherPlayer < closestPlayerDistance) {
            closestPlayerId = i;
            closestPlayerDistance = distanceToOtherPlayer;
        }
    }

    if (closestPlayerId == -1) {
        return;
    }
    
    attack(&client->m_aClients[closestPlayerId].m_Predicted.m_Pos);
}

void HookNearbyPlayerStrategy::attack(vec2* targetPos) {
    CCharacterCore* me = &client->m_PredictedChar;

    if (me->m_HookState == HOOK_RETRACTED) {
        getControls()->m_InputData.m_Hook = 0;
        return;
    }

    vec2* myPos = &me->m_Pos;
    if (distance(*myPos, *targetPos) > 350) {
        return;
    }

    //aim
    getControls()->m_MousePos.x = targetPos->x - myPos->x;
    getControls()->m_MousePos.y = targetPos->y - myPos->y;

    if (me->m_HookState == HOOK_GRABBED) {
        if (me->m_HookedPlayer != -1) {
            //hooked into someone already
            return;
        } else {
            //hooked into wall, release
            getControls()->m_InputData.m_Hook = 0;
            return;
        }
    } else {
        getControls()->m_InputData.m_Hook = 1;
    }

}