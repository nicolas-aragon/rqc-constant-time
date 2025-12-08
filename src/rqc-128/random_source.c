
#include "random_source.h"
#include <stdlib.h>


void random_source_init(random_source *o, int type) {
  o->type = type;

  if(type == RANDOM_SOURCE_PRNG) {
    o->ctx = NULL;
  } 

  if(type == RANDOM_SOURCE_SEEDEXP) {
    o->ctx = (AES_XOF_struct*) malloc(sizeof(AES_XOF_struct));
  }
}



void random_source_seed(random_source *o, unsigned char *seed) {
  if(o->type == RANDOM_SOURCE_PRNG) {
    randombytes_init(seed, NULL, 256); // Expecting 48 bits of entropy
  }

  if(o->type == RANDOM_SOURCE_SEEDEXP) {
    seedexpander_init(o->ctx, seed, seed + 32, 4294967295); // Expecting 32 + 8 bits of entropy
  }
}



void random_source_get_bytes(random_source *o, unsigned char *buffer, size_t len) {
  if(o->type == RANDOM_SOURCE_PRNG) {
    randombytes(buffer, len);
  } 

  if(o->type == RANDOM_SOURCE_SEEDEXP) {
    seedexpander(o->ctx, buffer, len);
  }
}



void random_source_clear(random_source *o) {
  if(o->type == RANDOM_SOURCE_SEEDEXP) {
    free(o->ctx);
  }
}

