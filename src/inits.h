//
//  inits.h
//  PlayInvaders
//
//  Created by Nick Bova on 4/25/23.
//

#ifndef inits_h
#define inits_h

#include <stdio.h>
#include "consts.h"

void initSound(GameState* state);
void checkInitGameOver(GameState* state);
void resetEnemyPosition(GameState* state);
void resetPlayerPosition(GameState* state);
void initGameRunning(GameState* state);
void initMainMenu(GameState* state);

#endif /* inits_h */
