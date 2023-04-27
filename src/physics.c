//
//  physics.c
//  PlayInvaders
//
//  Created by Nick Bova on 4/26/23.
//

#include "physics.h"
#include "inits.h"

void incrementLevel(GameState* state) {
    state->curr_level += 1;
    if (state->enemy_speed_x > 0) {
        state->enemy_speed_x += ENEMY_SPEED_INCREMENT;
    }
    else {
        state->enemy_speed_x -= ENEMY_SPEED_INCREMENT;
    }
    state->enemy_speed_y += ENEMY_SPEED_INCREMENT;
    state->curr_score_multiplier += SCORE_INCREMENT;
    
    resetEnemyPosition(state);
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

void shootEnemyBullets(GameState* state, int bulletPosX, int bulletPosY) {
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->enemy_bullet_pos_y[i] == INT32_MIN) {
            state->enemy_bullet_pos_x[i] = bulletPosX;
            state->enemy_bullet_pos_y[i] = bulletPosY;
            // Play enemy firing synth after firing bullet
            state->pd->sound->synth->playNote(state->enemy_fire_synth, 220, 1, 0.1, 0);
            break;
        }
    }
}

void shootBullets(GameState* state) {
    // Only shoot bullets if we haven't reached the max. Will do this by simply
    // checking to see if any bullets are set to INT32_MIN
    for (int i = 0; i < BULLET_MAX; i++) {
        if (state->bullet_pos_y[i] == INT32_MIN) {
            // Set the bullet at and above where the player is
            state->bullet_pos_x[i] = state->ship_pos_x + (PLAYER_WIDTH / 2);
            state->bullet_pos_y[i] = state->ship_pos_y - BULLET_HEIGHT;
            // After firing bullet, play play firing sound effect with corrresponding synth
            state->pd->sound->synth->playNote(state->player_fire_synth, 220, 1, 0.1, 0);
            // Break out of the loop (only shoot one bullet at a time)
            break;
        }
    }
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
                if (state->enemy_pos_y[j] != INT32_MIN) {
                    enemy.x = state->enemy_pos_x[j]; enemy.y = state->enemy_pos_y[j];
                    if (check_collision(bullet, enemy)) {
                        state->bullet_pos_x[i] = INT32_MIN; state->bullet_pos_y[i] = INT32_MIN;
                        state->enemy_pos_x[j] = INT32_MIN; state->enemy_pos_y[j] = INT32_MIN;
                        // Increment score after hit
                        state->curr_score += state->curr_score_multiplier;
                    }
                }
            }
        }
        if (state->bullet_pos_y[i] < 0) {
            state->bullet_pos_x[i] = INT32_MIN; state->bullet_pos_y[i] = INT32_MIN;
        }
        // Do the same for enemy bullets
        if (state->enemy_bullet_pos_y[i] != INT32_MIN) {
            state->enemy_bullet_pos_y[i] += ENEMY_BULLET_SPEED;
            
            if (state->enemy_bullet_pos_y[i] > MAX_HEIGHT) {
                state->enemy_bullet_pos_x[i] = INT32_MIN;
                state->enemy_bullet_pos_y[i] = INT32_MIN;
            }
            else {
                PDRect bullet, player;
                bullet.x = state->enemy_bullet_pos_x[i]; bullet.y = state->enemy_bullet_pos_y[i];
                bullet.width = BULLET_WIDTH; bullet.height = BULLET_HEIGHT;
                player.x = state->ship_pos_x; player.y = state->ship_pos_y;
                player.width = PLAYER_WIDTH; player.height = PLAYER_HEIGHT;
                if (check_collision(player, bullet)) {
                    // Remove bullet after collision. Check to see if game over state has been reached
                    state->enemy_bullet_pos_x[i] = INT32_MIN; state->enemy_bullet_pos_y[i] = INT32_MIN;
                    checkInitGameOver(state); break;
                }
            }
        }
    }
    
    // Keep track of the number of enemies still onscreen.
    int curr_num_enemies = 0;
    for (int i = 0; i < ENEMY_MAX; i++) {
        if (state->enemy_pos_y[i] != INT32_MIN) {
            curr_num_enemies += 1;
        }
    }
    //Increment the level and reset if there are no more enemies left
    if (curr_num_enemies == 0) {
        incrementLevel(state);
    }
    else {
        // If enough time has passed (ENEMY_MOVEMENT_FREQ), go through and move each
        // enemy still active
        int currTime = state->pd->system->getCurrentTimeMilliseconds();
        int allowEnemyFire = 0;
        // If enough time has passed (ENEMY_FIRING_INTERVAL), set flag to allow enemies to fire
        if (currTime - state->enemy_fire_time > ENEMY_FIRING_INTERVAL) {
            allowEnemyFire = 1;
        }
        if (currTime - state->enemy_move_time > ENEMY_MOVEMENT_FREQ - (ENEMY_MOVEMENT_FREQ_OFFSET * (ENEMY_MAX - curr_num_enemies))) {
            int speedFlipped = 0;
            // Perform initial check to see if any enemies have reached the left/right edges of the screen.
            // Use that to determine which direction to move them in this cycle
            for (int i = 0; i < ENEMY_MAX; i++) {
                if (state->enemy_pos_y[i] != INT32_MIN) {
                    if (!speedFlipped && (state->enemy_pos_x[i] + state->enemy_speed_x >= MAX_WIDTH - ENEMY_SCREEN_WIDTH_MARGIN - ENEMY_WIDTH
                        || state->enemy_pos_x[i] + state->enemy_speed_x <= ENEMY_SCREEN_WIDTH_MARGIN + ENEMY_WIDTH)) {
                        state->enemy_speed_x = -state->enemy_speed_x; speedFlipped = 1;
                        break;
                    }
                }
            }
            
            for (int i = 0; i < ENEMY_MAX; i++) {
                if (state->enemy_pos_y[i] != INT32_MIN) {
                    if (speedFlipped) {
                        state->enemy_pos_y[i] += state->enemy_speed_y;
                    }
                    else {
                        state->enemy_pos_x[i] += state->enemy_speed_x;
                    }
                    // If enemy reaches the bottom of the screen, set the game
                    // into a game over phase
                    if (state->enemy_pos_y[i] >= MAX_HEIGHT - PLAYER_HEIGHT - 15) {
                        checkInitGameOver(state); break;
                    }
                    // If we are allowing an enemy to fire, use a random number generated here to determine
                    // if the current enemy should fire. Only do this once per firing cycle.
                    if (allowEnemyFire) {
                        int fireNow = (rand() % 20) > 18;
                        if (fireNow) {
                            shootEnemyBullets(state, state->enemy_pos_x[i], state->enemy_pos_y[i]);
                            allowEnemyFire = 0;
                        }
                    }
                }
            }
            // Reset enemy move time
            state->enemy_move_time = currTime;
        }
    }
}
