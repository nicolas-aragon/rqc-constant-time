#include "rqc-128/rbc_43_augmented_gabidulin.h"
#include "rqc-128/rqc_128_parameters.h"
#include "time.h"

#define NUM_OF_TESTS 1 // 10000

static long long cpucycles()
{
  unsigned long long result;
  __asm__ volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax" : "=a"(result)::"%rdx");
  return result;
}

/**
 * \typedef rbc_43_extended_gabidulin
 * \brief Structure of a gabidulin code
 */
typedef struct rbc_43_gabidulin
{
  rbc_43_poly g; /**< Generator vector defining the code */
  uint32_t k;    /**< Size of vectors representing messages */
  uint32_t n;    /**< Size of vectors representing codewords */
} rbc_43_extended_gabidulin;

void rbc_43_extended_gabidulin_init(rbc_43_extended_gabidulin *code, const rbc_43_poly g, uint32_t k, uint32_t n);

void rbc_43_extended_gabidulin_encode(rbc_43_qre c, const rbc_43_extended_gabidulin gc, const rbc_43_vec m);

void rbc_43_extended_gabidulin_decode(rbc_43_vec m, const rbc_43_extended_gabidulin gc, const rbc_43_qre y);

int main(void)
{
  long long total_ag = 0, total_eg = 0, t0 = 0, t1 = 0;
  int cmp_ok1, cmp_ok2;

  rbc_43_vec m0, m1, m2, g;
  rbc_43_augmented_gabidulin ag_code;
  rbc_43_extended_gabidulin eg_code;
  rbc_43_qre c, e, y, g1;
  rbc_43_vspace supp_e;

  random_source prng;
  unsigned char buff[64];
  sprintf(buff, "%u", (unsigned)time(NULL));
  random_source_init(&prng, 0);
  random_source_seed(&prng, buff);

  printf("Error weight / AG / EG\n");

  rbc_43_field_init();
  rbc_43_qre_init_modulus(RQC_128_PARAM_N);

  rbc_43_qre_init(&y);
  rbc_43_qre_init(&c);
  rbc_43_vec_init(&g, RQC_128_PARAM_M);
  rbc_43_qre_init(&e);
  rbc_43_qre_init(&g1);

  rbc_43_vec_init(&m0, RQC_128_PARAM_K);
  rbc_43_vec_init(&m1, RQC_128_PARAM_K);
  rbc_43_vec_init(&m2, RQC_128_PARAM_K);
  for (int error_weight = RQC_128_PARAM_W; error_weight >= 0; error_weight--)
  {
    total_ag = 0;
    total_eg = 0;
    rbc_43_vspace_init(&supp_e, error_weight);
    for (int i = 0; i < NUM_OF_TESTS; i++)
    {
      rbc_43_vec_set_random(&prng, m0, RQC_128_PARAM_K);
      rbc_43_vec_set(m1, m0, RQC_128_PARAM_K);
      rbc_43_vec_set(m2, m0, RQC_128_PARAM_K);

      rbc_43_vec_set_random_full_rank(&prng, g, RQC_128_PARAM_M);
      rbc_43_qre_set_random_full_rank(&prng, g1);

      rbc_43_augmented_gabidulin_init(&ag_code, g, RQC_128_PARAM_K, RQC_128_PARAM_N, RQC_128_PARAM_M, RQC_128_PARAM_EPSILON);
      rbc_43_extended_gabidulin_init(&eg_code, g1, RQC_128_PARAM_K, RQC_128_PARAM_N);

      rbc_43_vspace_set_random_full_rank(&prng, supp_e, error_weight);
      rbc_43_qre_set_random_from_support(&prng, e, supp_e, error_weight, 1);

      rbc_43_extended_gabidulin_encode(c, eg_code, m2);
      rbc_43_qre_add(y, c, e);
      t0 = cpucycles();
      rbc_43_extended_gabidulin_decode(m2, eg_code, y);
      t1 = cpucycles();

      total_eg += t1 - t0;

      rbc_43_augmented_gabidulin_encode(c, ag_code, m1);
      rbc_43_qre_add(y, c, e);
      t0 = cpucycles();
      rbc_43_augmented_gabidulin_decode(m1, ag_code, y->v);
      t1 = cpucycles();

      total_ag += t1 - t0;

      cmp_ok1 = 1;
      cmp_ok2 = 1;

      for (int j = 0; j < RQC_128_PARAM_K; j++)
      {
        cmp_ok1 &= rbc_43_elt_is_equal_to(m0[j], m1[j]);
        cmp_ok2 &= rbc_43_elt_is_equal_to(m0[j], m2[j]);
      }

      if (cmp_ok1 == 0)
      {
        printf("The decoded messages from AG code are not equal %u\n");
        return EXIT_FAILURE;
      }
      if (cmp_ok2 == 0)
      {
        printf("The decoded messages from EG code are not equal %u\n");
        return EXIT_FAILURE;
      }

      for (int j = 0; j < RQC_128_PARAM_K; j++)
      {
        if (rbc_43_elt_is_equal_to(m0[j], m1[j]) == 0)
        {
          printf("AG decoding not working\n");
          return EXIT_FAILURE;
        }
      }
    }
    rbc_43_vspace_clear(supp_e);

    printf("%d %llu  %llu\n", error_weight, total_ag / NUM_OF_TESTS, total_eg / NUM_OF_TESTS);
  }
  rbc_43_qre_clear(c);
  rbc_43_qre_clear(y);
  rbc_43_qre_clear(e);
  rbc_43_vec_clear(g);
  rbc_43_qre_clear_modulus();
  random_source_clear(&prng);
  return EXIT_SUCCESS;
}

/**
 * \fn void rbc_43_extended_gabidulin_init(rbc_43_gabidulin* code, const rbc_43_poly g, uint32_t k, uint32_t n)
 * \brief This function initializes a gabidulin code.
 *
 * \param[in] code Gabidulin code
 * \param[in] g Generator vector defining the code
 * \param[in] k Size of vectors representing messages
 * \param[in] n Size of vectors representing codewords
 */
void rbc_43_extended_gabidulin_init(rbc_43_extended_gabidulin *code, const rbc_43_poly g, uint32_t k, uint32_t n)
{
  code->g = g;
  code->k = k;
  code->n = n;
}

/**
 * \fn void rbc_43_extended_gabidulin_encode(rbc_43_qre c, const rbc_43_gabidulin gc, const rbc_43_vec m)
 * \brief This function encodes a message into a codeword.
 *
 * \param[out] c Vector of size <b>n</b> representing a codeword
 * \param[in] gc Gabidulin code
 * \param[in] m Vector of size <b>k</b> representing a message
 */
void rbc_43_extended_gabidulin_encode(rbc_43_qre c, const rbc_43_extended_gabidulin gc, const rbc_43_vec m)
{
  // Compute generator matrix
  rbc_43_elt matrix[gc.k][gc.n];
  for (size_t j = 0; j < gc.n; ++j)
  {
    rbc_43_elt_set(matrix[0][j], gc.g->v[j]);
    for (size_t i = 1; i < gc.k; ++i)
    {
      rbc_43_elt_sqr(matrix[i][j], matrix[i - 1][j]);
    }
  }

  // Encode message
  rbc_43_elt tmp;
  rbc_43_elt_set_zero(tmp);
  rbc_43_qre_set_zero(c);
  for (size_t i = 0; i < gc.k; ++i)
  {
    for (size_t j = 0; j < gc.n; ++j)
    {
      rbc_43_elt_mul(tmp, m[i], matrix[i][j]);
      rbc_43_elt_add(c->v[j], c->v[j], tmp);
    }
  }

#ifdef VERBOSE
  printf("\n\n\n# Extended Gabidulin Encoding - Begin #");
  printf("\n\ng: ");
  rbc_43_poly_print(gc.g);

  printf("\n\nmatrix:[ ");
  for (size_t i = 0; i < gc.k; ++i)
  {
    printf("[ ");
    for (size_t j = 0; j < gc.n; ++j)
    {
      rbc_43_elt_print(matrix[i][j]);
    }
    printf("] ");
  }
  printf("]\n");

  printf("\ncodeword: ");
  rbc_43_poly_print(c);
  printf("\n\n# Extended Gabidulin Encoding - End #\n");
#endif
}

void rbc_43_extended_gabidulin_decode(rbc_43_vec m, const rbc_43_extended_gabidulin gc, const rbc_43_qre y)
{

  /*
   *  Step 1: Solving the q-polynomial reconstruction2 problem
   */

  uint32_t t = (gc.n - gc.k) / 2;
  int32_t max_degree_N = ((gc.n - gc.k) % 2 == 0) * (gc.k + t - 1) + ((gc.n - gc.k) % 2 == 1) * (gc.k + t);

  rbc_43_qpoly A, I, N0, N1, V0, V1, N1_result, V1_result;
  rbc_43_qpoly_init(&A, max_degree_N);
  rbc_43_qpoly_init(&I, t);
  rbc_43_qpoly_init(&N0, max_degree_N + 1);
  rbc_43_qpoly_init(&N1, max_degree_N);
  rbc_43_qpoly_init(&V0, max_degree_N);
  rbc_43_qpoly_init(&V1, t);
  rbc_43_qpoly_init(&N1_result, max_degree_N);
  rbc_43_qpoly_init(&V1_result, t);

  rbc_43_qpoly qtmp1, qtmp2, qtmp3, qtmp4, qtmp5;
  rbc_43_qpoly_init(&qtmp1, max_degree_N);
  rbc_43_qpoly_init(&qtmp2, max_degree_N + 1);
  rbc_43_qpoly_init(&qtmp3, max_degree_N);
  rbc_43_qpoly_init(&qtmp4, max_degree_N);
  rbc_43_qpoly_init(&qtmp5, t);

  rbc_43_vec u0, u1, rand;
  rbc_43_vec_init(&u0, gc.n);
  rbc_43_vec_init(&u1, gc.n);
  rbc_43_vec_init(&rand, gc.n);
  random_source prng;
  random_source_init(&prng, RANDOM_SOURCE_PRNG);
  rbc_43_vec_set_random(&prng, rand, gc.n);

  rbc_43_elt e1, e2, tmp1, tmp2, tmp3, tmp4;
  uint32_t status = 0;

  // Initialization step

  // A(g[i]) = 0 for 0 <= i <= k - 1
  // I(g[i]) = y[i] for 0 <= i <= k - 1

  rbc_43_qpoly_set_interpolate_vect_and_zero(A, I, (rbc_43_vec)gc.g->v, (rbc_43_vec)y->v, gc.k);

  rbc_43_qpoly_set(N0, A);
  rbc_43_qpoly_set(N1, I);
  rbc_43_qpoly_set_zero(V0);
  rbc_43_qpoly_set_one(V1);

  rbc_43_qpoly_set_zero(N1_result);
  rbc_43_qpoly_set_one(V1_result);

  rbc_43_qpoly_set_zero(qtmp1);
  rbc_43_qpoly_set_zero(qtmp2);
  rbc_43_qpoly_set_zero(qtmp3);
  rbc_43_qpoly_set_zero(qtmp4);
  rbc_43_qpoly_set_zero(qtmp5);

  // u0[i] = A(g[i]) - V0(y[i])
  // u1[i] = I(g[i]) - V1(y[i])

  rbc_43_vec_set_zero(u0, gc.k);
  rbc_43_vec_set_zero(u1, gc.k);

  for (size_t i = gc.k; i < gc.n; ++i)
  {
    rbc_43_qpoly_evaluate2(tmp1, A, gc.g->v[i], gc.k);
    rbc_43_elt_set(u0[i], tmp1);

    rbc_43_qpoly_evaluate2(tmp1, I, gc.g->v[i], gc.k - 1);
    rbc_43_elt_add(tmp1, tmp1, y->v[i]);
    rbc_43_elt_set(u1[i], tmp1);

    status = status | !rbc_43_elt_is_zero(u1[i]);
  }

#ifdef VERBOSE
  printf("\n\n# Extended Gabidulin Decoding - Begin #");
  printf("\n\ng: ");
  rbc_43_poly_print(gc.g);
  printf("\n\nA: ");
  rbc_43_qpoly_print(A);
  printf("\nI: ");
  rbc_43_qpoly_print(I);
  printf("\nN0 (init): ");
  rbc_43_qpoly_print(N0);
  printf("\nN1 (init): ");
  rbc_43_qpoly_print(N1);
  printf("\nV0 (init): ");
  rbc_43_qpoly_print(V0);
  printf("\nV1 (init): ");
  rbc_43_qpoly_print(V1);
#endif

  // Interpolation step
  for (size_t i = gc.k; i < gc.n; ++i)
  {
    uint32_t bound1 = (i - gc.k) / 2;     // deg(N0) <= k + bound1, deg(V1) <= bound1
    uint32_t bound2 = (i - gc.k + 1) / 2; // deg(V0) <= bound2, deg(N1)<= k - 1 + bound2
    uint32_t next = i;
    uint32_t r = 1;

    for (size_t k = i; k < gc.n; ++k)
    {
      r &= rbc_43_elt_is_zero(u1[k]);
      next = r * (next + 1) + (1 - r) * next;
    }

    status = status & (next != gc.n);
    next = (next == gc.n) * (next - 1) + (1 - (next == gc.n)) * next;

    // Permutation of the coordinates of positions i and next

    for (size_t k = i; k < gc.n; k++)
    {
      rbc_43_elt_set_mask1(tmp3, u0[k], tmp3, (k == next));
      rbc_43_elt_set_mask1(tmp4, u1[k], tmp4, (k == next));
    }
    rbc_43_elt_set(tmp1, u0[i]);
    rbc_43_elt_set_mask1(u0[i], u0[next], rand[i], status);
    rbc_43_elt_set_mask2(u0[next], rand[i], tmp1, status);

    rbc_43_elt_set(tmp2, u1[i]);
    rbc_43_elt_set_mask1(u1[i], u1[next], rand[i], status);
    rbc_43_elt_set_mask2(u1[next], rand[i], tmp2, status);

    for (size_t k = i; k < gc.n; k++)
    {
      rbc_43_elt_set_mask1(u0[k], tmp3, u0[k], (k == next));
      rbc_43_elt_set_mask1(u1[k], tmp4, u1[k], (k == next));
    }

    // Update q_polynomials according to discrepancies

    // e1 = - u1[i]^q / u1[i]
    // e2 = - u0[i] / u1[i]
    // N0' = N1^q - e1.N1
    // V0' = V1^q - e1.V1
    // N1' = N0 - e2.N1
    // V1' = V0 - e2.V1

    rbc_43_elt_inv(tmp1, u1[i]);
    rbc_43_elt_sqr(e1, u1[i]);
    rbc_43_elt_mul(e1, e1, tmp1);
    rbc_43_elt_mul(e2, u0[i], tmp1);

    rbc_43_qpoly_scalar_mul2(qtmp1, N1, e1, gc.k - 1 + bound2);
    rbc_43_qpoly_qexp2(qtmp2, N1, gc.k - 1 + bound2);

    rbc_43_qpoly_scalar_mul2(qtmp4, V1, e1, bound1);
    rbc_43_qpoly_qexp2(qtmp5, V1, bound1);

    rbc_43_qpoly_scalar_mul2(N1, N1, e2, gc.k - 1 + bound2);
    rbc_43_qpoly_add2(N1, N0, N1, gc.k + bound1, gc.k - 1 + bound2);

    rbc_43_qpoly_scalar_mul2(V1, V1, e2, bound1);
    rbc_43_qpoly_add2(V1, V0, V1, bound2, bound1);

    rbc_43_qpoly_add2(N0, qtmp1, qtmp2, gc.k - 1 + bound2, gc.k + bound2);
    rbc_43_qpoly_add2(V0, qtmp4, qtmp5, qtmp4->max_degree, qtmp5->max_degree);

    rbc_43_qpoly_set_mask(N1_result, N1, N1_result, gc.k + bound1, status);
    rbc_43_qpoly_set_mask(V1_result, V1, V1_result, bound2, status);

    // Update discrepancies
    for (size_t k = i + 1; k < gc.n; ++k)
    {

      // u0[k]' = u1[k]^q - e1.u1[k]
      // u1[k]' = u0[k] - e2.u1[k]

      rbc_43_elt_mul(tmp1, e1, u1[k]);
      rbc_43_elt_sqr(tmp2, u1[k]);
      rbc_43_elt_add(tmp1, tmp1, tmp2);

      rbc_43_elt_mul(tmp2, e2, u1[k]);
      rbc_43_elt_add(tmp2, tmp2, u0[k]);
      rbc_43_elt_set(u1[k], tmp2);

      rbc_43_elt_set(u0[k], tmp1);
    }

#ifdef VERBOSE
    printf("\n");
    printf("\nN0 (%zu): ", i);
    rbc_43_qpoly_print(N0);
    printf("\nN1 (%zu): ", i);
    rbc_43_qpoly_print(N1);
    printf("\nV0 (%zu): ", i);
    rbc_43_qpoly_print(V0);
    printf("\nV1 (%zu): ", i);
    rbc_43_qpoly_print(V1);
#endif
  }

  /*
   *  Step 2: Computing f (qtmp1 variable)
   */

  // rbc_43_qpoly_mul2(qtmp1, N1_result, A, t - 1, gc.k);

  uint32_t cmp_deg_N1 = (rbc_43_qpoly_get_degree(N1_result) == -1); // 1 if true
  uint32_t cmp_deg_V1 = (rbc_43_qpoly_get_degree(V1_result) == 0);

  // If both are true then status = 0 otherwise status = 1
  status = 1 - (cmp_deg_N1 & cmp_deg_V1);

  rbc_43_qpoly_set_mask(qtmp1, N1_result, A, max_degree_N, status);
  rbc_43_qpoly_set_mask(V1_result, V1_result, V1, t, status);

  int32_t deg1, deg2;
  deg1 = rbc_43_qpoly_get_degree(qtmp1);
  deg2 = rbc_43_qpoly_get_degree(V1_result);

  rbc_43_qpoly_left_div2(qtmp3, qtmp4, qtmp1, V1_result, deg1, deg2);
  rbc_43_qpoly_set_mask(qtmp1, qtmp3, I, t, status);

  /*
   *  Step 3: Decoding the message as the value of the k first coordinates of f (qtmp1 variable)
   */

  rbc_43_vec_set(m, qtmp1->values, gc.k);

#ifdef VERBOSE
  printf("\nquotient: ");
  rbc_43_qpoly_print(qtmp1);
  printf("\nremainder: ");
  rbc_43_qpoly_print(qtmp2);
  printf("\nmu: ");
  rbc_43_vec_print(m, gc.k);
  printf("\n# Extended Gabidulin Decoding - End #\n");
#endif

  rbc_43_qpoly_clear(A);
  rbc_43_qpoly_clear(I);

  rbc_43_qpoly_clear(N0);
  rbc_43_qpoly_clear(N1);
  rbc_43_qpoly_clear(V0);
  rbc_43_qpoly_clear(V1);

  rbc_43_qpoly_clear(N1_result);
  rbc_43_qpoly_clear(V1_result);

  rbc_43_qpoly_clear(qtmp1);
  rbc_43_qpoly_clear(qtmp2);
  rbc_43_qpoly_clear(qtmp3);
  rbc_43_qpoly_clear(qtmp4);
  rbc_43_qpoly_clear(qtmp5);

  rbc_43_vec_clear(u0);
  rbc_43_vec_clear(u1);
  rbc_43_vec_clear(rand);
  random_source_clear(&prng);
}