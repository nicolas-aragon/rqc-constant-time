#include "rqc-128/rbc_43_augmented_gabidulin.h"
#include "rqc-128/rqc_128_parameters.h"
#include <time.h>

#define NUM_OF_TESTS 10000

int main(void)
{
  decoding_time_t t;
  decoding_time_t total = {.gauss = 0, .annihilator = 0, .evaluation = 0, .loidreau1 = 0, .loidreau2 = 0, .leftdiv1 = 0, .leftdiv2 = 0};

  rbc_43_vec m1, m2, g;
  rbc_43_augmented_gabidulin code;
  rbc_43_qre c, e, tmp;
  rbc_43_vspace supp_e;

  int cmp_ok;

  random_source prng;
  random_source_init(&prng, 0);

  rbc_43_field_init();
  rbc_43_qre_init_modulus(RQC_128_PARAM_N1 * RQC_128_PARAM_N2);

  rbc_43_qre_init(&tmp);
  rbc_43_qre_init(&c);
  rbc_43_vec_init(&g, RQC_128_PARAM_M);
  rbc_43_qre_init(&e);

  rbc_43_vec_init(&m1, RQC_128_PARAM_K);
  rbc_43_vec_init(&m2, RQC_128_PARAM_K);

  for (int weight = RQC_128_PARAM_W; weight >= RQC_128_PARAM_W; --weight)
  {
    total = (decoding_time_t){.gauss = 0, .annihilator = 0, .evaluation = 0, .loidreau1 = 0, .loidreau2 = 0, .leftdiv1 = 0, .leftdiv2 = 0};
    rbc_43_vspace_init(&supp_e, weight);
    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
      rbc_43_vec_set_random(&prng, m1, RQC_128_PARAM_K);
      rbc_43_vec_set(m2, m1, RQC_128_PARAM_K);
      rbc_43_vec_set_random_full_rank(&prng, g, RQC_128_PARAM_M);

      rbc_43_augmented_gabidulin_init(&code, g, RQC_128_PARAM_K, RQC_128_PARAM_N1 * RQC_128_PARAM_N2, RQC_128_PARAM_M, RQC_128_PARAM_EPSILON);

      rbc_43_vspace_set_random_full_rank(&prng, supp_e, weight);
      rbc_43_qre_set_random_from_support(&prng, e, supp_e, weight, 1);

      rbc_43_augmented_gabidulin_encode(c, code, m1);
      rbc_43_qre_add(tmp, c, e);
      t = rbc_43_augmented_gabidulin_decode(m1, code, tmp->v);

      total.gauss += t.gauss;
      total.annihilator += t.annihilator;
      total.evaluation += t.evaluation;
      total.loidreau1 += t.loidreau1;
      total.loidreau2 += t.loidreau2;
      total.leftdiv1 += t.leftdiv1;
      total.leftdiv2 += t.leftdiv2;

      cmp_ok = 1;

      for (int j = 0; j < RQC_128_PARAM_K; j++)
      {
        cmp_ok &= rbc_43_elt_is_equal_to(m1[j], m2[j]);
      }

      if (cmp_ok == 0)
      {
        printf("The decoded messages are not equal %u\n");
        return EXIT_FAILURE;
      }
    }
    rbc_43_vspace_clear(supp_e);

    printf("Average CPU cycles over %d trials for error weight %d:\n", NUM_OF_TESTS, weight);
    printf("Reduction - Echelonization: %lld CPU cycles\n", total.gauss / NUM_OF_TESTS);
    printf("Reduction - V2 (annihilator): %lld CPU cycles\n", total.annihilator / NUM_OF_TESTS);
    printf("Reduction - Evaluation: %lld CPU cycles\n", total.evaluation / NUM_OF_TESTS);
    printf("Loidreau - Initialisation: %lld CPU cycles\n", total.loidreau1 / NUM_OF_TESTS);
    printf("Loidreau - Interpolation: %lld CPU cycles\n", total.loidreau2 / NUM_OF_TESTS);
    printf("First left division: %lld CPU cycles\n", total.leftdiv1 / NUM_OF_TESTS);
    printf("Second left division: %lld CPU cycles\n", total.leftdiv2 / NUM_OF_TESTS);
    printf("TOTAL: %llu CPU cycles\n", (total.gauss + +total.annihilator + total.evaluation + total.loidreau1 + total.loidreau2 + total.leftdiv1 + total.leftdiv2) / NUM_OF_TESTS);
  }

  // rbc_43_vspace_clear(supp_e);
  rbc_43_qre_clear(c);
  rbc_43_qre_clear(tmp);
  rbc_43_qre_clear(e);
  rbc_43_vec_clear(g);
  rbc_43_qre_clear_modulus();
  random_source_clear(&prng);

  return EXIT_SUCCESS;
}
