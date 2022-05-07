#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

#define PLAYER_HEIGHT 10
#define PLAYER_WIDTH 50
#define NUM_BULLET_MAX 2

#define PLAYER_SPEED 5
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
        
        pd->display->setRefreshRate(30);
		// Note: If you set an update callback in the kEventInit handler, the system assumes the game is pure C and doesn't run any Lua code in the game
		pd->system->setUpdateCallback(update, state);
	}
	
	return 0;
}

void shootBullets(GameState* state) {
    
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

void renderAssets(GameState* state) {
    state->pd->graphics->clear(kColorWhite);
    state->pd->system->drawFPS(0,0);
    state->pd->graphics->fillRect(state->ship_pos_x, state->ship_pos_y, PLAYER_WIDTH, PLAYER_HEIGHT, kColorBlack);
}

static int update(void* userdata) {
	GameState* state = userdata;
	
    checkButtons(state);
    renderAssets(state);
    
	return 1;
}

