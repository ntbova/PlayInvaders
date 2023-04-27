//
//  gfx.c
//  PlayInvaders
//
//  Created by Nick Bova on 4/25/23.
//

#include "gfx.h"
#include "consts.h"
#include "helpers.h"

void renderAssets(GameState* state) {
    // Don't render assets if we've switched to a game over or other phase
    if (state->curr_phase != pGameRunning) { return; }
    
    state->pd->system->drawFPS(0,0);
    state->pd->graphics->fillRect(state->ship_pos_x, state->ship_pos_y, PLAYER_WIDTH, PLAYER_HEIGHT, kColorBlack);
    // Render bullets on screen
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->bullet_pos_y[i] != INT32_MIN) {
            state->pd->graphics->fillRect(state->bullet_pos_x[i], state->bullet_pos_y[i], BULLET_WIDTH, BULLET_HEIGHT, kColorBlack);
        }
        if (state->enemy_bullet_pos_y[i] != INT32_MIN) {
            state->pd->graphics->fillRect(state->enemy_bullet_pos_x[i], state->enemy_bullet_pos_y[i], BULLET_WIDTH, BULLET_HEIGHT, kColorBlack);
        }
    }
    // Render enemies on screen
    for (int i = 0; i < ENEMY_MAX; i++) {
        if (state->enemy_pos_x[i] != INT32_MIN) {
            state->pd->graphics->fillRect(state->enemy_pos_x[i], state->enemy_pos_y[i], ENEMY_WIDTH, ENEMY_HEIGHT, kColorBlack);
        }
    }
    // Render score
    // Get number of digits in the score
    int digits = num_places(state->curr_score);
    char scoreStr[digits]; get_dec_str(scoreStr, digits, state->curr_score);
    state->pd->graphics->drawText(scoreStr, digits, kASCIIEncoding, SCORE_POS_X, SCORE_POS_Y);
    // Render level
    digits = num_places(state->curr_level);
    char levelStr[digits]; get_dec_str(levelStr, digits, state->curr_level);
    state->pd->graphics->drawText(levelStr, digits, kASCIIEncoding, LEVEL_POS_X, LEVEL_POS_Y);
    // Render number of lives
    digits = LIVES_MAX_DIGITS; char livesStr[digits];
    get_dec_str(livesStr, digits, state->curr_lives);
    state->pd->graphics->drawText(livesStr, digits, kASCIIEncoding, LIVES_POS_X, LIVES_POS_Y);
}

LCDBitmap* loadBitmapFromPath(PlaydateAPI* pd, char* path) {
    const char *err = NULL;
    struct LCDBitmap* bmp = pd->graphics->loadBitmap(path, &err);
    if ( err != NULL ) { pd->system->logToConsole("Error loading image: %s", err); }
    return bmp;
}

LCDSprite* loadSpriteFromBitmap(PlaydateAPI* pd, LCDBitmap* bmp, LCDBitmapFlip flip) {
    struct LCDSprite* pathSprite = pd->sprite->newSprite();
    pd->sprite->setImage(pathSprite, bmp, flip);
    return pathSprite;
}
