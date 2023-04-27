#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pd_api.h"
#include "inits.h"
#include "consts.h"
#include "game.h"

int eventHandler(PlaydateAPI* pd, PDSystemEvent event, uint32_t arg) {
	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit ) {
        GameState* state = pd->system->realloc(NULL,sizeof(GameState));
        state->pd = pd;
        
        // Fonts
        state->title_font = state->pd->graphics->loadFont("fonts/BebasNeue-Regular-48", NULL);
        state->score_font = state->pd->graphics->loadFont("fonts/galvaniz-20", NULL);
        state->pd->graphics->setFont(state->title_font);
        
        // Sound
        initSound(state);
        
        state->curr_phase = pMainMenu;
        state->enemy_move_time = state->pd->system->getCurrentTimeMilliseconds();
        state->enemy_fire_time = state->pd->system->getCurrentTimeMilliseconds();
        state->curr_score = 0;
        
        pd->display->setRefreshRate(MAX_FRAMERATE);
		pd->system->setUpdateCallback(update, state);
	}
	
	return 0;
}
