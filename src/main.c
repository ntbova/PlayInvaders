#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pd_api.h"

#define PLAYER_HEIGHT 10
#define PLAYER_WIDTH 50
#define BULLET_HEIGHT 5
#define BULLET_WIDTH 5
#define ENEMY_HEIGHT 18
#define ENEMY_WIDTH 18
#define BULLET_MAX 2
#define ENEMY_MAX 26
#define MAX_FRAMERATE 30
#define PLAYER_SPEED 5
#define PLAYER_CRANK_SPEED 0.5f
#define BULLET_SPEED 5
#define ENEMY_SPEED 5
#define MAX_HEIGHT 240
#define MAX_WIDTH 400
#define SCREEN_MARGIN 24
#define ENEMY_MARGIN_WIDTH 30
#define ENEMY_MARGIN_HEIGHT 5
#define ENEMY_MOVEMENT_FREQ 1000 // ms
#define SCORE_MULTIPLIER 10
#define SCORE_POS_X 360
#define SCORE_POS_Y 5

static int update(void* userdata);

enum GamePhases {
    pMainMenu = 0,
    pGameRunning = 1,
    pGameOver = -1
};

typedef struct GameStates {
    PlaydateAPI* pd;
    LCDFont* title_font;
    LCDFont* score_font;
    enum GamePhases curr_phase;
    uint32_t curr_score;
    int ship_pos_x;
    int ship_pos_y;
    int bullet_pos_x[BULLET_MAX];
    int bullet_pos_y[BULLET_MAX];
    int enemy_pos_x[ENEMY_MAX];
    int enemy_pos_y[ENEMY_MAX];
    int enemy_move_time;
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

void initGameRunning(GameState* state) {
    state->curr_phase = pGameRunning;
    
    state->ship_pos_x = MAX_WIDTH; state->ship_pos_x /= 2;
    state->ship_pos_y = MAX_HEIGHT; state->ship_pos_y -= 15;
    
    for (int i = 0; i < BULLET_MAX; i++) {
        state->bullet_pos_x[i] = INT32_MIN; state->bullet_pos_y[i] = INT32_MIN;
        state->enemy_pos_x[i] = INT32_MIN; state->enemy_pos_y[i] = INT32_MIN;
    }
    
    int currEnemyPosX = SCREEN_MARGIN; int currEnemyPosY = SCREEN_MARGIN;
    for (int i = 0; i < ENEMY_MAX; i++) {
        // For layout of enemies on screen, start from the top of the screen with a certain margin,
        // if we reach the end of the screen, then reset the x position and start a new row
        if (currEnemyPosX < MAX_WIDTH - SCREEN_MARGIN) {
            state->enemy_pos_x[i] = currEnemyPosX; state->enemy_pos_y[i] = currEnemyPosY;
            currEnemyPosX += ENEMY_MARGIN_WIDTH + ENEMY_WIDTH;
        } else {
            currEnemyPosX = SCREEN_MARGIN; currEnemyPosY += ENEMY_MARGIN_HEIGHT + ENEMY_HEIGHT;
        }
    }
    
    state->pd->graphics->setFont(state->score_font);
}

void initGameOver(GameState* state) {
    state->curr_phase = pGameOver;
    state->pd->graphics->setFont(state->title_font);
}

int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg) {
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit ) {
        GameState* state = pd->system->realloc(NULL,sizeof(GameState));
        state->pd = pd;
        
        // Fonts
        state->title_font = state->pd->graphics->loadFont("fonts/BebasNeue-Regular-48", NULL);
        state->score_font = state->pd->graphics->loadFont("fonts/galvaniz-20", NULL);
        state->pd->graphics->setFont(state->title_font);
        
        state->curr_phase = pMainMenu;
        state->enemy_move_time = state->pd->system->getCurrentTimeMilliseconds();
        state->curr_score = 0;
        
        pd->display->setRefreshRate(MAX_FRAMERATE);
		pd->system->setUpdateCallback(update, state);
	}
	
	return 0;
}

int num_places(int n) {
    int r = 1;
    if (n < 0) n = (n == INT32_MIN) ? INT32_MAX: -n;
    while (n > 9) {
        n /= 10;
        r++;
    }
    return r;
}

void get_dec_str(char* str, size_t len, uint32_t val)
{
    uint8_t i;
    for(i=1; i<=len; i++)
    {
        str[len-i] = (uint8_t) ((val % 10UL) + '0');
        val /= 10;
    }
    str[i-1] = '\0';
}


int check_collision(PDRect a, PDRect b) {
    //The sides of the rectangles
    int leftA, leftB, rightA, rightB;
    int topA, topB, bottomA, bottomB;

    //Calculate the sides of rect A
    leftA = a.x; rightA = a.x + a.width;
    topA = a.y; bottomA = a.y + a.height;

    //Calculate the sides of rect B
    leftB = b.x; rightB = b.x + b.width;
    topB = b.y; bottomB = b.y + b.height;
    //If any of the sides from A are outside of B
    if( bottomA <= topB ) { return 0; }
    if( topA >= bottomB ) { return 0; }
    if( rightA <= leftB ) { return 0; }
    if( leftA >= rightB ) { return 0; }

    //If none of the sides from A are outside B
    return 1;
}

void shootBullets(GameState* state) {
    // Only shoot bullets if we haven't reached the max. Will do this by simply
    // checking to see if any bullets are set to INT32_MIN
    for (int i = 0; i < BULLET_MAX; i++) {
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
    
    if (pushed & kButtonB || pushed & kButtonUp) { shootBullets(state); }
}

void checkCrank(GameState* state) {
    float change = state->pd->system->getCrankChange();
    int newPlayPosX = state->ship_pos_x + (change * PLAYER_CRANK_SPEED);
    if (newPlayPosX < 0) { newPlayPosX = 0; }
    else if (newPlayPosX > MAX_WIDTH - PLAYER_WIDTH) { newPlayPosX = MAX_WIDTH - PLAYER_WIDTH; }
    state->ship_pos_x = newPlayPosX;
}

void moveAssets(GameState* state) {
    // Go through each bullet currently on the screen, decrease y-axis value.
    // Once the bullet leaves the boundries of the screen, reset its positional
    // value to INT32_MIN
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->bullet_pos_y[i] != INT32_MIN) {
            state->bullet_pos_y[i] -= BULLET_SPEED;
            // After moving bullet, check to see if it collides with any of the enemies.
            // Remove them both in this case.
            PDRect bullet, enemy;
            bullet.x = state->bullet_pos_x[i]; bullet.y = state->bullet_pos_y[i];
            bullet.width = BULLET_WIDTH; bullet.height = BULLET_HEIGHT;
            enemy.width = ENEMY_WIDTH; enemy.height = ENEMY_HEIGHT;
            for (int j = 0; j < ENEMY_MAX; j++) {
                enemy.x = state->enemy_pos_x[j]; enemy.y = state->enemy_pos_y[j];
                if (check_collision(bullet, enemy)) {
                    state->bullet_pos_x[i] = INT32_MIN; state->bullet_pos_y[i] = INT32_MIN;
                    state->enemy_pos_x[j] = INT32_MIN; state->enemy_pos_y[j] = INT32_MIN;
                    // Increment score after hit
                    state->curr_score += SCORE_MULTIPLIER;
                }
            }
        }
        if (state->bullet_pos_y[i] < 0) {
            state->bullet_pos_x[i] = INT32_MIN; state->bullet_pos_y[i] = INT32_MIN;
        }
    }
    
    // If enough time has passed (ENEMY_MOVEMENT_FREQ), go through and move each
    // enemy still active
    int currTime = state->pd->system->getCurrentTimeMilliseconds();
    if (currTime - state->enemy_move_time > ENEMY_MOVEMENT_FREQ) {
        for (int i = 0; i < ENEMY_MAX; i++) {
            if (state->enemy_pos_y[i] != INT32_MIN) {
                state->enemy_pos_y[i] += ENEMY_SPEED;
                // If enemy reaches the bottom of the screen, set the game
                // into a game over phase
                if (state->enemy_pos_y[i] >= MAX_HEIGHT) {
                    initGameOver(state); break;
                }
            }
        }
        // Reset enemy move time
        state->enemy_move_time = currTime;
    }
}

void renderAssets(GameState* state) {
    
    state->pd->graphics->clear(kColorWhite);
    
    // Don't render assets if we've switched to a game over or other phase
    if (state->curr_phase != pGameRunning) { return; }
    
    state->pd->system->drawFPS(0,0);
    state->pd->graphics->fillRect(state->ship_pos_x, state->ship_pos_y, PLAYER_WIDTH, PLAYER_HEIGHT, kColorBlack);
    // Render bullets on screen
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->bullet_pos_y[i] != INT32_MIN) {
            state->pd->graphics->fillRect(state->bullet_pos_x[i], state->bullet_pos_y[i], BULLET_WIDTH, BULLET_HEIGHT, kColorBlack);
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
}

static int update(void* userdata) {
	GameState* state = userdata;
	
    switch (state->curr_phase)
    {
        case pMainMenu:
            mainMenuLoop(state);
            break;
        case pGameRunning:
            checkButtons(state);
            checkCrank(state);
            moveAssets(state);
            renderAssets(state);
            break;
        case pGameOver:
            gameOverLoop(state);
            break;
    }
    
	return 1;
}

