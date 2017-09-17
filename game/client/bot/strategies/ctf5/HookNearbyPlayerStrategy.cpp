#include "HookNearbyPlayerStrategy.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <base/vmath.h>
#include <game/client/components/chat.h>
#include <game/collision.h>
#include <game/gamecore.h>
HookNearbyPlayerStrategy::HookNearbyPlayerStrategy(CGameClient* client) :
BotStrategy(client) {
  spotIndex = 0;
  maxYPos = 3000;
  chatTime = 0;
  spotTime = 0;
  hookTime = 0;
}

// TODO: Split into several strategies, or rename?

void HookNearbyPlayerStrategy::execute() {

    CCharacterCore* me = &client->m_PredictedChar;
    int myId = client->m_Snap.m_LocalClientID;
    int spots[5] = {2000, 4450, 6974, 1850, 7100};

    int myTeam = client->m_aClients[myId].m_Team;
  // m_Snap.m_pGameDataObj->m_FlagCarrierRed == FLAG_TAKEN

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

        /*
        if (otherPlayer->m_HookState == HOOK_GRABBED && otherPlayer->m_HookedPlayer == -1) {
          continue;
        }
        */

        float distanceToOtherPlayer = distance(me->m_Pos, otherPlayer->m_Pos);



        if((client->m_Snap.m_pGameDataObj->m_FlagCarrierBlue == i || client->m_Snap.m_pGameDataObj->m_FlagCarrierRed == i) && distanceToOtherPlayer < 350) {
          // WE WANT HIM!
          closestPlayerId = i;
          closestPlayerDistance = distanceToOtherPlayer;
          break;
        }
        if (distanceToOtherPlayer < closestPlayerDistance) {
            closestPlayerId = i;
            closestPlayerDistance = distanceToOtherPlayer;
        }
    }

    CCharacterCore* target = &client->m_aClients[closestPlayerId].m_Predicted;
    vec2* myPos = &me->m_Pos;
    vec2* myVel = &me->m_Vel;

    if(myPos->y > maxYPos) {
      maxYPos = myPos->y;
      printf("MaxYPos: %d\n", maxYPos);
    }

    if (me->m_HookState == HOOK_GRABBED) {
        if (me->m_HookedPlayer != -1) {
            //hooked into someone already
            getControls()->m_InputData.m_Jump = 0;
            if(myPos->y > (maxYPos - 200)) {
              if (target->m_HookState == HOOK_GRABBED && (target->m_HookedPlayer == -1)) {

              } else {
                if (getNowMillis() < chatTime + 1000) {

              	} else {
                  sprintf(chatMessage, "%s: %s", &client->m_aClients[closestPlayerId].m_aName, getRandomLine());
                  client->m_pChat->Say(0, chatMessage);
                  chatTime = getNowMillis();
                }
              }
            }
        }
    }

    if (closestPlayerId == -1) {
      // To towards selected spot of map
      //printf("Want to go to: %d\n", spots[spotIndex]);
      if (myPos->x < spots[spotIndex]) {
        go(true, true, 0);
      } else if(myPos->x > spots[spotIndex]) {
        go(false, true, 0);
      }
      // Pick a different target
      if(abs(myPos->x - spots[spotIndex]) < 10 || (getNowMillis() > spotTime + 15000)) {
        spotIndex = (spotIndex + 1) % 5;
        spotTime = getNowMillis();
      }
      if(myVel->y > 0) {
        getControls()->m_InputData.m_Jump = 1;
      } else {
        getControls()->m_InputData.m_Jump = 0;
      }
      return;
    }



    vec2* targetPos = &target->m_Pos;
    vec2* targetVel = &target->m_Pos;
    if(distance(*myPos, *targetPos) < 350) {

      if (me->m_HookState == HOOK_GRABBED && me->m_HookedPlayer == -1) {
        getControls()->m_InputData.m_Hook = 0;
      } else {

        if (target->m_HookState == HOOK_GRABBED && target->m_HookedPlayer == -1) {
          // He's trying to stay on ground! Move in his direction
          //getControls()->m_InputData.m_Hook = 0;
          if(myPos->x < ( (target->m_HookPos.x + myPos->x) / 2  )) {
            go(true, false, 0);
          } else {
            go(false, false, 0);
          }

        } else {

          // Approach nearest pit
          if(targetPos->x < 1795) {
            go(false, false, 0);
          } else if(targetPos->x > 1795 && targetPos->x < 3141) {
            go(true, false, 0);
          } else if(targetPos->x > 3232 && targetPos->x < 4487) {
            go(false, false, 0);
          }  else if(targetPos->x > 4487 && targetPos->x < 5742) {
            go(true, false, 0);
          } else if(targetPos->x > 5833 && targetPos->x < 7179) {
            go(false, false, 0);
          } else if(targetPos->x > 7179) {
            go(true, false, 0);
          } else if(targetVel->x < 0) {
            go(true, false, 0);
          } else {
            go(false, false, 0);
          }

        }

      }

      if (!(target->m_HookState == HOOK_GRABBED && target->m_HookedPlayer == -1 && targetVel->x < 5)) {
        attack(&client->m_aClients[closestPlayerId].m_Predicted.m_Pos);
      }


    } else {

        // Approach target!
        if(targetPos->x < myPos->x) {
          go(false, true, targetPos->y);
        } else {
          go(true, true, targetPos->y);
        }

    }

    if((me->m_HookState != HOOK_GRABBED) && myVel->y >= 0 && targetPos->y < myPos->y) {
      getControls()->m_InputData.m_Jump = 1;
    } else {
      getControls()->m_InputData.m_Jump = 0;
    }



}

void HookNearbyPlayerStrategy::go(bool right, bool withHooking, float yPos) {
  CCharacterCore* me = &client->m_PredictedChar;
  float velThreshold;

  if(right) {
    getControls()->m_InputDirectionRight = 1;
    getControls()->m_InputDirectionLeft = 0;
  } else {
    getControls()->m_InputDirectionRight = 0;
    getControls()->m_InputDirectionLeft = 1;
  }
  if(withHooking) {
    if (me->m_HookState == HOOK_RETRACTED) {
      getControls()->m_InputData.m_Hook = 0;
      return;
    }

    if((fabs(me->m_Vel.x) < 0.2f && fabs(me->m_Vel.y) < 0.2f)) {
      //getControls()->m_InputData.m_Hook = 0;
      spotIndex = (spotIndex + 1) % 5;
      spotTime = getNowMillis();
      return;
    }

    if(me->m_HookState == HOOK_GRABBED && me->m_HookedPlayer == -1) {
      //printf("HOOK DISTANCE: %f\n", distance(me->m_HookPos, me->m_Pos));
      if(getNowMillis() > (hookTime + 2000)) {
        getControls()->m_InputData.m_Hook = 0;
        return;
      }
      float hookLength = distance(me->m_HookPos, me->m_Pos);
      if(hookLength < 200) {
        if(hookLength < 100 || (me->m_Pos.x > (me->m_HookPos.x+32) && right) || (me->m_Pos.x < (me->m_HookPos.x-32) && !right)) {

          velThreshold = 20.0f * fabs(3886.0f/2.0f - (float)me->m_Pos.y) / (3886.0f/2.0f);
        //  printf("Threshold: %f, Vel: %f\n", velThreshold, me->m_Vel.y);

          if(
               (yPos && ( (me->m_Pos.y - yPos) > 0 && me->m_Vel.y > velThreshold))
            || (yPos && ( (me->m_Pos.y - yPos) < 0 && -me->m_Vel.y > velThreshold))
            || (   me->m_Vel.y  > velThreshold && (me->m_HookPos.y - me->m_Pos.y) > 0)
            || ( (-me->m_Vel.y) > velThreshold && (me->m_HookPos.y - me->m_Pos.y) < 0)) {
            // Release. We will shoot next tick
          //  printf("OK!\n");
            getControls()->m_InputData.m_Hook = 0;
            return;
          }
        }
      }

    }


    if (me->m_HookState != HOOK_GRABBED) {
      if(!me->m_pCollision) return;
      float xCandidatesNormal[8] = {
        2, 1, 0.5f, 3, 4, 5, 6, 7
      };
      float xCandidatesDanger[15] = {
        2, 1, 0.5f, 3, 4, 5, 6, 7, -2, -1, -3, -4, -5, -6, -7
      };

      //printf("%f\n", me->m_Pos.y );
      bool isDanger = true // TEMP?
      && me->m_Pos.y > 4000;
      float *xCandidates = isDanger ? xCandidatesDanger : xCandidatesNormal;
      if (isDanger || getNowMillis() > (hookTime + 200)) {
        float candidateX;
        float candidateY;

        for(int i = 0; i < (isDanger ? 15 : 8); i++) {
          if(me->m_Vel.y >= 0) {
            candidateX = right?xCandidates[i]:-xCandidates[i];
            candidateY= -1;
          } else {
            candidateX = right?xCandidates[i]:-xCandidates[i];
            candidateY= 1;
          }
          vec2 hookDir = normalize(vec2(candidateX, candidateY));
          float steps = 25.0f;
          for(float hookLength = 0.0f; hookLength < 300.0f; hookLength+=steps) {
            vec2 NewPos = me->m_HookPos + hookDir * hookLength;
            int Hit = me->m_pCollision->IntersectLine(me->m_Pos, NewPos, &NewPos, (vec2*)NULL);
            if(distance(me->m_Pos, NewPos) < 100) continue;
        		if(Hit) {
        			if(Hit&CCollision::COLFLAG_NOHOOK) {

              } else {
                getControls()->m_MousePos.x = candidateX;
                getControls()->m_MousePos.y = candidateY;
                getControls()->m_InputData.m_Hook = 1;
                hookTime = getNowMillis();
                return;
              }
            }
          }
        }
      }
    }

  }
}

char *HookNearbyPlayerStrategy::getRandomLine() {
  char* lines[4] = {
    ":)", "Noob", "Haha", "Got you"
  };
  return lines[rand() % 4];
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
    if (me->m_HookState != HOOK_GRABBED) {
      getControls()->m_MousePos.x = targetPos->x - myPos->x;
      getControls()->m_MousePos.y = targetPos->y - myPos->y;
      getControls()->m_InputData.m_Hook = 1;
    }


}
