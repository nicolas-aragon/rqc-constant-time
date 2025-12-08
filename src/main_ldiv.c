#include "rqc-128/rbc_43_elt.h"
#include "rqc-128/rbc_43_qpoly.h"
#include "rqc-128/rqc_128_parameters.h"
#include <time.h>

#define NUM_OF_TESTS 10000

#define dA 42


static long long cpucycles() {
  unsigned long long result;
  __asm__ volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax" : "=a" (result) ::  "%rdx");
  return result;
}


// Test if the two input q-polynomials are equal
uint8_t rbc_qpoly_is_equal_to(rbc_43_qpoly p1, rbc_43_qpoly p2) {
  if (p1->degree != p2->degree) {
    return 0;
  }
  
  for (int32_t i = 0; i <= p1->degree; i++) {
    if (rbc_43_elt_is_equal_to(p1->values[i], p2->values[i]) == 0) {
      return 0;
    }
  }
  return 1;
}

// Generate a random q-polynomial of q-degree <= degree
void random_qpoly(random_source *prng, rbc_43_qpoly poly, int32_t degree) {
  poly->degree = -1;
  for (int i = 0; i <= degree; i++) {
     rbc_43_elt_set_random(prng, poly->values[i]);
     if (rbc_43_elt_is_zero(poly->values[i]) == 0) {
        poly->degree++;
     }
  }
}


int main(void) {
  long long total_ct = 0;
  long long tmp;
  random_source prng;
  unsigned char buff[64];
  sprintf(buff, "%u", (unsigned) time(NULL));
  random_source_init(&prng, RANDOM_SOURCE_PRNG);
  random_source_seed(&prng, buff);
  rbc_43_qpoly A, B, Q, R, Q0;
  rbc_43_qpoly_init(&A, dA);
  rbc_43_qpoly_init(&R, dA);
  rbc_43_qpoly_init(&B, dA);
  rbc_43_qpoly_init(&Q, dA);
  rbc_43_qpoly_init(&Q0, dA);
  for(int dB = dA; dB > 0; dB--){
    total_ct = 0;
    for (int i = 0; i < NUM_OF_TESTS; i++) {
      rbc_43_qpoly_set_zero(R);
      rbc_43_qpoly_set_zero(A);
      rbc_43_qpoly_set_zero(B);
      random_qpoly(&prng, B, dB);
      random_qpoly(&prng, Q0, dA-dB);
      rbc_43_qpoly_mul2(A, B, Q0, dB, dA-dB);
      tmp = cpucycles();
      rbc_43_qpoly_left_div2(Q, R, A, B, rbc_43_qpoly_get_degree(A), rbc_43_qpoly_get_degree(B));
      total_ct += cpucycles() - tmp;
      if(!rbc_qpoly_is_equal_to(Q0, Q) || rbc_43_qpoly_get_degree(R)!=-1) {
        printf("division didn't works.\n");
        return EXIT_FAILURE;
      }
    }
    printf("%u %lld\n", dB, total_ct/NUM_OF_TESTS);
  }
  rbc_43_qpoly_clear(B);
  rbc_43_qpoly_clear(Q);
  rbc_43_qpoly_clear(Q0);
  rbc_43_qpoly_clear(A);
  rbc_43_qpoly_clear(R);
  random_source_clear(&prng);

  return EXIT_SUCCESS;
}