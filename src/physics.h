//
//  physics.h
//  PlayInvaders
//
//  Created by Nick Bova on 4/26/23.
//

#ifndef physics_h
#define physics_h

#include <stdio.h>
#include "pd_api.h"
#include "consts.h"

void incrementLevel(GameState* state);
int check_collision(PDRect a, PDRect b);
void shootEnemyBullets(GameState* state, int bulletPosX, int bulletPosY);
void shootBullets(GameState* state);
void moveAssets(GameState* state);

#endif /* physics_h */
