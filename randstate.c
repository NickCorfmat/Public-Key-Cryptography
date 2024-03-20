#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>
#include <stdint.h>

#include "randstate.h"

gmp_randstate_t state;

// INITIALIZES the global random state 'state' using 'seed' as the random seed.
void randstate_init(uint64_t seed) {
    srandom(seed);

    gmp_randinit_mt(state);
    gmp_randseed_ui(state, seed);
}

// DEALLOCATES all memory used by the global random state, 'state'.
void randstate_clear(void) {
    gmp_randclear(state);
}
