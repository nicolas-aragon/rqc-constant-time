#include "rqc-128/rbc_43_elt.h"
#include "rqc-128/rbc_43_qpoly.h"
#include "rqc-128/rbc_43_augmented_gabidulin.h"
#include "rqc-128/rbc_43_qre.h"
#include "rqc-128/rbc_43_vec.h"
#include "rqc-128/rqc_128_parameters.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_OF_TESTS 10000

static unsigned long long cpucycles() {
  unsigned long long result;
  __asm__ volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax" : "=a" (result) ::  "%rdx");
  return result;
}

int main(void) {
  unsigned long long t0, total; 
  rbc_43_vec m1, m2, g;
  rbc_43_augmented_gabidulin code;
  rbc_43_qre c, e, y;
  rbc_43_vspace supp_e;
  
  random_source prng;
  unsigned char buff[64];
  sprintf(buff, "%u", (unsigned) time(NULL));
  random_source_init(&prng, 0);
  random_source_seed(&prng, buff);

  rbc_43_field_init();
  rbc_43_qre_init_modulus(RQC_128_PARAM_N1 * RQC_128_PARAM_N2);

  rbc_43_qre_init(&y);
  rbc_43_qre_init(&c);
  rbc_43_qre_init(&e);

  rbc_43_vec_init(&g, RQC_128_PARAM_M);
  rbc_43_vec_init(&m1, RQC_128_PARAM_K);
  rbc_43_vec_init(&m2, RQC_128_PARAM_K);

  for(int error_weight = RQC_128_PARAM_W; error_weight >= 0; error_weight--){

    total = 0; //Total CPU cycles over the NUM_OF_TESTS iterations of decoding
    rbc_43_vspace_init(&supp_e, error_weight);

    for (int i = 0; i < NUM_OF_TESTS; i++) {
      rbc_43_vec_set_random(&prng, m1, RQC_128_PARAM_K);
      rbc_43_vec_set(m2, m1, RQC_128_PARAM_K);

      rbc_43_vec_set_random_full_rank(&prng, g, RQC_128_PARAM_M);
      
      rbc_43_augmented_gabidulin_init(&code, g, RQC_128_PARAM_K, RQC_128_PARAM_N1*RQC_128_PARAM_N2, RQC_128_PARAM_M, RQC_128_PARAM_EPSILON);

      rbc_43_vspace_set_random_full_rank(&prng, supp_e, error_weight);
      rbc_43_qre_set_random_from_support(&prng, e, supp_e, error_weight, 1);

      rbc_43_augmented_gabidulin_encode(c, code, m1); // c = m1 * G
      rbc_43_qre_add(y, c, e); // y = c + e

      t0 = cpucycles();
      rbc_43_augmented_gabidulin_decode(m2, code, y->v); // m2 = Decode(y)
      total += cpucycles() - t0;

      for (int j = 0; j < RQC_128_PARAM_K; j++) {
        if (rbc_43_elt_is_equal_to(m1[j], m2[j]) == 0) { // (m1 == m2) ?
          printf("The decoded messages are not equal %u\n", i);
          return EXIT_FAILURE;
        } 
      }
    }
  rbc_43_vspace_clear(supp_e);

  printf("%u %llu\n", error_weight, total/NUM_OF_TESTS);
  }
  rbc_43_qre_clear(c);
  rbc_43_qre_clear(y);
  rbc_43_qre_clear(e);

  rbc_43_vec_clear(g);
  rbc_43_vec_clear(m1);
  rbc_43_vec_clear(m2);

  rbc_43_qre_clear_modulus();
  random_source_clear(&prng);

  return EXIT_SUCCESS;
} 
