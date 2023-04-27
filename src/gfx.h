//
//  gfx.h
//  PlayInvaders
//
//  Created by Nick Bova on 4/25/23.
//

#ifndef gfx_h
#define gfx_h

#include <stdio.h>
#include "pd_api.h"
#include "consts.h"

void renderAssets(GameState* state);
LCDBitmap* loadBitmapFromPath(PlaydateAPI* pd, char* path);
LCDSprite* loadSpriteFromBitmap(PlaydateAPI* pd, LCDBitmap* bmp, LCDBitmapFlip flip);

#endif /* gfx_h */
