//
//  input.c
//  PlayInvaders
//
//  Created by Nick Bova on 4/25/23.
//

#include "input.h"
#include "consts.h"
#include "pd_api.h"
#include "physics.h"

void checkButtons(GameState* state) {
    PDButtons current;
    PDButtons pushed;
    
    state->pd->system->getButtonState(&current, &pushed, NULL);
    
    if (current & kButtonRight) {
        if (state->ship_pos_x < MAX_WIDTH - PLAYER_WIDTH ) {
            state->ship_pos_x += PLAYER_SPEED;
        }
    }
    else if (current & kButtonLeft) {
        if (state->ship_pos_x > 0 ) {
            state->ship_pos_x -= PLAYER_SPEED;
        }
    }
    
    if (pushed & kButtonA || pushed & kButtonB || pushed & kButtonUp) { shootBullets(state); }
}

void checkCrank(GameState* state) {
    float change = state->pd->system->getCrankChange();
    int newPlayPosX = state->ship_pos_x + (change * PLAYER_CRANK_SPEED);
    if (newPlayPosX < 0) { newPlayPosX = 0; }
    else if (newPlayPosX > MAX_WIDTH - PLAYER_WIDTH) { newPlayPosX = MAX_WIDTH - PLAYER_WIDTH; }
    state->ship_pos_x = newPlayPosX;
}
