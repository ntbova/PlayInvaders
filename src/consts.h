//
//  consts.h
//  PlayInvaders
//
//  Created by Nick Bova on 4/25/23.
//

#ifndef consts_h
#define consts_h

#include "pd_api.h"

#define PLAYER_HEIGHT 10
#define PLAYER_WIDTH 50
#define BULLET_HEIGHT 5
#define BULLET_WIDTH 5
#define ENEMY_HEIGHT 18
#define ENEMY_WIDTH 18
#define BULLET_MAX 2
#define ENEMY_MAX 32
#define MAX_FRAMERATE 50
#define PLAYER_SPEED 5
#define PLAYER_CRANK_SPEED 0.5f
#define BULLET_SPEED 5
#define ENEMY_BULLET_SPEED 2
#define MAX_HEIGHT 240
#define MAX_WIDTH 400
#define SCREEN_MARGIN 36
#define ENEMY_STARTING_SPEED 5
#define ENEMY_SPEED_INCREMENT 1
#define ENEMY_MARGIN_WIDTH 24
#define ENEMY_SCREEN_WIDTH_MARGIN 10
#define ENEMY_MARGIN_HEIGHT 5
#define ENEMY_MOVEMENT_FREQ 1000 // ms
#define ENEMY_MOVEMENT_FREQ_OFFSET 32
#define ENEMY_FIRING_INTERVAL 3000 // ms
#define SCORE_STARTING_MULTIPLIER 5
#define SCORE_INCREMENT 1
#define TIME_DURING_GAME_OVER 5
#define STARTING_LIVES 3
#define LIVES_MAX_DIGITS 1
#define SCORE_POS_X 360
#define SCORE_POS_Y 3
#define LEVEL_POS_X 15
#define LEVEL_POS_Y 3
#define LIVES_POS_X 200
#define LIVES_POS_Y 3

enum GamePhases {
    pMainMenu = 0,
    pGameRunning = 1,
    pGameOver = -1
};

typedef struct GameStates {
    PlaydateAPI* pd;
    LCDFont* title_font;
    LCDFont* score_font;
    PDSynth* player_fire_synth;
    PDSynth* enemy_fire_synth;
    enum GamePhases curr_phase;
    uint32_t curr_score;
    uint32_t curr_score_multiplier;
    uint8_t curr_level;
    uint8_t curr_lives;
    int ship_pos_x;
    int ship_pos_y;
    int bullet_pos_x[BULLET_MAX];
    int bullet_pos_y[BULLET_MAX];
    int enemy_pos_x[ENEMY_MAX];
    int enemy_pos_y[ENEMY_MAX];
    int enemy_bullet_pos_x[BULLET_MAX];
    int enemy_bullet_pos_y[BULLET_MAX];
    int enemy_speed_x;
    int enemy_speed_y;
    int enemy_move_time;
    int enemy_fire_time;
} GameState;

#endif /* consts_h */
