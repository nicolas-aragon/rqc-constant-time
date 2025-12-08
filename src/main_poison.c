#include "rqc-128/rbc_43_augmented_gabidulin.h"
#include "rqc-128/rqc_128_parameters.h"
#include "time.h"

#include "valgrind/memcheck.h"
#include "rqc-128/poisoning.h"

#define NUM_OF_TEST 10000

int main(void)
{
  rbc_43_vec m1, m2, g;
  rbc_43_augmented_gabidulin code;
  rbc_43_qre c, e, tmp;
  rbc_43_vspace support_e;

  random_source prng;
  // unsigned char buff[64];
  // sprintf(buff, "%u", (unsigned) time(NULL));
  random_source_init(&prng, 0);
  // random_source_seed(&prng, buff);

  rbc_43_field_init();
  rbc_43_qre_init_modulus(RQC_128_PARAM_N);

  rbc_43_qre_init(&tmp);
  rbc_43_qre_init(&c);
  rbc_43_qre_init(&e);
  rbc_43_vec_init(&g, RQC_128_PARAM_M);
  rbc_43_vec_init(&m1, RQC_128_PARAM_K);
  rbc_43_vec_init(&m2, RQC_128_PARAM_K);

  for (int error_weight = RQC_128_PARAM_W; error_weight >= 0; error_weight--)
  {
    rbc_43_vspace_init(&support_e, error_weight);
    for (int i = 0; i < NUM_OF_TEST; i++)
    {
      rbc_43_vec_set_random(&prng, m1, RQC_128_PARAM_K);
      rbc_43_vec_set_random_full_rank(&prng, g, RQC_128_PARAM_M);

      rbc_43_augmented_gabidulin_init(&code, g, RQC_128_PARAM_K, RQC_128_PARAM_N1 * RQC_128_PARAM_N2, RQC_128_PARAM_M, RQC_128_PARAM_EPSILON);

      rbc_43_vspace_set_random_full_rank(&prng, support_e, error_weight);
      rbc_43_qre_set_random_from_support(&prng, e, support_e, error_weight, 1);
      rbc_43_augmented_gabidulin_encode(c, code, m1);
      rbc_43_qre_add(tmp, c, e);
      rbc_43_augmented_gabidulin_decode(m2, code, tmp->v);

      int cmp_ok = 1;

      for (int j = 0; j < RQC_128_PARAM_K; j++)
      {
        cmp_ok &= rbc_43_elt_is_equal_to(m1[j], m2[j]);
      }

      unpoison(&cmp_ok, 4); // At this stage we output if the test was succesful
      if (cmp_ok == 0)
      {
        printf("The decoded messages are not equal %u\n");
        return EXIT_FAILURE;
      }
    }

    rbc_43_vspace_clear(support_e);

    printf("Decoding error of weight %u: done.\n", error_weight);
  }
  rbc_43_qre_clear(c);
  rbc_43_qre_clear(tmp);
  rbc_43_qre_clear(e);
  rbc_43_vec_clear(g);
  rbc_43_vec_clear(m1);
  rbc_43_vec_clear(m2);
  rbc_43_qre_clear_modulus();
  random_source_clear(&prng);

  return EXIT_SUCCESS;
}