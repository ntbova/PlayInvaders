//
//  helpers.c
//  PlayInvaders
//
//  Created by Nick Bova on 4/25/23.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"
#include "consts.h"

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
    for(i=1; i<=len; i++) {
        str[len-i] = (uint8_t) ((val % 10UL) + '0');
        val /= 10;
    }
    str[i-1] = '\0';
}
