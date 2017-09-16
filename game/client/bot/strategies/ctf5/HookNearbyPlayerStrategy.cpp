#include "HookNearbyPlayerStrategy.h"
#include <stdio.h>
#include <stdlib.h>
HookNearbyPlayerStrategy::HookNearbyPlayerStrategy(CGameClient* client) :
BotStrategy(client) {
}

// TODO: Split into several strategies, or rename?

void HookNearbyPlayerStrategy::execute() {

    CCharacterCore* me = &client->m_PredictedChar;
    int myId = client->m_Snap.m_LocalClientID;

    int myTeam = client->m_aClients[myId].m_Team;

    int closestPlayerId = -1;
    float closestPlayerDistance = 999999;

    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (i == myId || !client->m_Snap.m_aCharacters[i].m_Active)
            continue;

        CCharacterCore* otherPlayer = &client->m_aClients[i].m_Predicted;

        if(myTeam != client->m_aClients[i].m_Team) {
          // We don't attack other team :)
          continue;
        }

        float distanceToOtherPlayer = distance(me->m_Pos, otherPlayer->m_Pos);
        if (distanceToOtherPlayer < closestPlayerDistance) {
            closestPlayerId = i;
            closestPlayerDistance = distanceToOtherPlayer;
        }
    }

    vec2* myPos = &me->m_Pos;
    vec2* myVel = &me->m_Vel;

    if (me->m_HookState == HOOK_GRABBED) {
        if (me->m_HookedPlayer != -1) {
            //hooked into someone already
            getControls()->m_InputData.m_Jump = 0;
        } else {
            //hooked into wall, release
            getControls()->m_InputData.m_Hook = 0;
            getControls()->m_InputData.m_Jump = 1;
        }
    }

    if (closestPlayerId == -1) {
      // To towards middle of map
      if (myPos->x < 4487) {
        getControls()->m_InputDirectionRight = 1;
        getControls()->m_InputDirectionLeft = 0;
      } else if(myPos->x > 4487) {
        getControls()->m_InputDirectionRight = 0;
        getControls()->m_InputDirectionLeft = 1;
      }
      if(myVel->y >= 0) {
        getControls()->m_InputData.m_Jump = 1;
      } else {
        getControls()->m_InputData.m_Jump = 0;
      }
      return;
    }


    CCharacterCore* target = &client->m_aClients[closestPlayerId].m_Predicted;
    vec2* targetPos = &target->m_Pos;
    vec2* targetVel = &target->m_Pos;
    if(distance(*myPos, *targetPos) < 350) {
      if(targetPos->x < 1795) {
        getControls()->m_InputDirectionRight = 0;
        getControls()->m_InputDirectionLeft = 1;
      } else if(targetPos->x > 1795 && targetPos->x < 3141) {
        getControls()->m_InputDirectionRight = 1;
        getControls()->m_InputDirectionLeft = 0;
      } else if(targetPos->x > 3232 && targetPos->x < 4487) {
        getControls()->m_InputDirectionRight = 0;
        getControls()->m_InputDirectionLeft = 1;
      }  else if(targetPos->x > 4487 && targetPos->x < 5742) {
        getControls()->m_InputDirectionRight = 1;
        getControls()->m_InputDirectionLeft = 0;
      } else if(targetPos->x > 5833 && targetPos->x < 7179) {
        getControls()->m_InputDirectionRight = 0;
        getControls()->m_InputDirectionLeft = 1;
      } else if(targetPos->x > 7179) {
        getControls()->m_InputDirectionRight = 1;
        getControls()->m_InputDirectionLeft = 0;
      } else if(targetVel->x < 0) {
        getControls()->m_InputDirectionRight = 1;
        getControls()->m_InputDirectionLeft = 0;
      } else {
        getControls()->m_InputDirectionRight = 0;
        getControls()->m_InputDirectionLeft = 1;
      }

    } else {

      // Approach target!
      if(targetPos->x < myPos->x) {
        getControls()->m_InputDirectionRight = 0;
        getControls()->m_InputDirectionLeft = 1;
      } else {
        getControls()->m_InputDirectionRight = 1;
        getControls()->m_InputDirectionLeft = 0;
      }

    }

    if(myVel->y >= 0 && targetPos->y < myPos->y) {
      getControls()->m_InputData.m_Jump = 1;
    } else {
      getControls()->m_InputData.m_Jump = 0;
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
    int t = 650;
    if( abs(myPos->x - 350) > t
     && abs(myPos->x - 1795) > t
     && abs(myPos->x - 3141) > t
     && abs(myPos->x - 3232) > t
     && abs(myPos->x - 7179) > t
     && abs(myPos->x - 5833) > t
     && abs(myPos->x - 5742) > t
     && abs(myPos->x - 8624) > t
     ) {
       return;
     }
    if (distance(*myPos, *targetPos) > 350) {
        return;
    }

    //aim
    getControls()->m_MousePos.x = targetPos->x - myPos->x;
    getControls()->m_MousePos.y = targetPos->y - myPos->y;
    if (me->m_HookState != HOOK_GRABBED) {
        getControls()->m_InputData.m_Hook = 1;
    }


}
