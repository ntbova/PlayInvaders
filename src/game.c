//
//  game.c
//  PlayInvaders
//
//  Created by Nick Bova on 4/25/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pd_api.h"
#include "consts.h"
#include "input.h"
#include "inits.h"
#include "physics.h"
#include "gfx.h"

#ifdef _WINDLL
__declspec(dllexport)
#endif

void mainGameLoop(GameState* state) {
    checkButtons(state);
    checkCrank(state);
    moveAssets(state);
    renderAssets(state);
}

void mainMenuLoop(GameState* state) {
    char* title = "Play Invaders";
    state->pd->graphics->drawText(title, strlen(title), kASCIIEncoding, 48, 50);
    
    PDButtons pushed;
    state->pd->system->getButtonState(NULL, &pushed, NULL);
    if (pushed & kButtonA || pushed & kButtonB) {
        initGameRunning(state);
    }
}

void gameOverLoop(GameState* state) {
    char* title = "Game Over";
    state->pd->graphics->drawText(title, strlen(title), kASCIIEncoding, 64, 50);
    // If enough time has passed, go back to main menu
    if (state->pd->system->getElapsedTime() > TIME_DURING_GAME_OVER) {
        initMainMenu(state);
    }
}

int update(void* userdata) {
    GameState* state = userdata;
    state->pd->graphics->clear(kColorWhite);
    
    switch (state->curr_phase)
    {
        case pMainMenu:
            mainMenuLoop(state);
            break;
        case pGameRunning:
            mainGameLoop(state);
            break;
        case pGameOver:
            gameOverLoop(state);
            break;
    }
    
    return 1;
}
