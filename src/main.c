#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

#define PLAYER_HEIGHT 10
#define PLAYER_WIDTH 50
#define BULLET_HEIGHT 5
#define BULLET_WIDTH 5
#define NUM_BULLET_MAX 2
#define MAX_FRAMERATE 50

#define PLAYER_SPEED 5
#define BULLET_SPEED 5
#define MAX_HEIGHT 240
#define MAX_WIDTH 400

static int update(void* userdata);

typedef struct GameStates {
    PlaydateAPI* pd;
    int ship_pos_x;
    int ship_pos_y;
    int bullet_pos_x[NUM_BULLET_MAX];
    int bullet_pos_y[NUM_BULLET_MAX];
} GameState;

#ifdef _WINDLL
__declspec(dllexport)
#endif

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

int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg) {
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit ) {
        GameState* state = (GameState*) malloc(sizeof(GameState));
        state->pd = pd;
        state->ship_pos_x = MAX_WIDTH; state->ship_pos_x /= 2;
        state->ship_pos_y = MAX_HEIGHT; state->ship_pos_y /= 2;
        
        for (int i = 0; i < NUM_BULLET_MAX; i++) {
            state->bullet_pos_x[i] = INT32_MIN; state->bullet_pos_y[i] = INT32_MIN;
        }
        
        pd->display->setRefreshRate(MAX_FRAMERATE);
		pd->system->setUpdateCallback(update, state);
	}
	
	return 0;
}

void shootBullets(GameState* state) {
    // Only shoot bullets if we haven't reached the max. Will do this by simply
    // checking to see if any bullets are set to INT32_MIN
    for (int i = 0; i < NUM_BULLET_MAX; i++) {
        if (state->bullet_pos_y[i] == INT32_MIN) {
            // Set the bullet at and above where the player is
            state->bullet_pos_x[i] = state->ship_pos_x + (PLAYER_WIDTH / 2);
            state->bullet_pos_y[i] = state->ship_pos_y - BULLET_HEIGHT;
            // Break out of the loop (only shoot one bullet at a time)
            break;
        }
    }
}

void checkButtons(GameState* state) {
    PDButtons current;
    PDButtons pushed;
    
    state->pd->system->getButtonState(&current, &pushed, NULL);
    
    int speed = PLAYER_SPEED;

    if (current & kButtonDown) { state->ship_pos_y += speed; }
    else if (current & kButtonUp) { state->ship_pos_y -= speed; }
    if (current & kButtonRight) { state->ship_pos_x += speed; }
    else if (current & kButtonLeft) { state->ship_pos_x -= speed; }
    
    if (pushed & kButtonB) { shootBullets(state); }
}

void moveAssets(GameState* state) {
    // Go through each bullet currently on the screen, decrease y-axis value.
    // Once the bullet leaves the boundries of the screen, reset its positional
    // value to INT32_MIN
    for (int i = 0; i < NUM_BULLET_MAX; i++) {
        if (state->bullet_pos_y[i] != INT32_MIN) {
            state->bullet_pos_y[i] -= BULLET_SPEED;
        }
        if (state->bullet_pos_y[i] < 0) {
            state->bullet_pos_x[i] = INT32_MIN; state->bullet_pos_y[i] = INT32_MIN;
        }
    }
}

void renderAssets(GameState* state) {
    state->pd->graphics->clear(kColorWhite);
    state->pd->system->drawFPS(0,0);
    state->pd->graphics->fillRect(state->ship_pos_x, state->ship_pos_y, PLAYER_WIDTH, PLAYER_HEIGHT, kColorBlack);
    
    for (int i = 0; i < NUM_BULLET_MAX; i++) {
        if (state->bullet_pos_y[i] != INT32_MIN) {
            state->pd->graphics->fillRect(state->bullet_pos_x[i], state->bullet_pos_y[i], BULLET_WIDTH, BULLET_HEIGHT, kColorBlack);
        }
    }
}

static int update(void* userdata) {
	GameState* state = userdata;
	
    checkButtons(state);
    moveAssets(state);
    renderAssets(state);
    
	return 1;
}

