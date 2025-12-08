/**
 * \file augmented_gabidulin.c
 * \brief Implementation of augmented_gabidulin.h
 *
 * The decoding algorithm provided is based on rbc_43_qpolys reconstruction, see \cite gabidulin:welch and \cite gabidulin:generalized for details.
 *
 */

#include "rbc_43_vspace.h"
#include "rbc_43_augmented_gabidulin.h"
#include "rqc_128_parameters.h"

#ifdef VALGRIND
#include "poisoning.h"
#endif

#ifdef TIME
static unsigned long long cpucycles()
{
  unsigned long long result;
  __asm__ volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax" : "=a"(result)::"%rdx");
  return result;
}
#endif

/**
 * \fn void rbc_43_augmented_gabidulin_init(rbc_43_augmented_gabidulin* code, const rbc_43_poly g, uint32_t k, uint32_t n, uint32_t nn, uint32_t epsilon)
 * \brief This function initializes an augmented gabidulin code.
 *
 * \param[in] code Augmented Gabidulin code
 * \param[in] g Generator vector defining the code
 * \param[in] k Size of vectors representing messages
 * \param[in] n Size of vectors representing codewords
 * \param[in] nn Size of vectors representing codewords
 * \param[in] epsilon Lower-bound of the dimension of the support erasure
 */
void rbc_43_augmented_gabidulin_init(rbc_43_augmented_gabidulin *code, const rbc_43_vec g, uint32_t k, uint32_t n, uint32_t nn, uint32_t epsilon)
{
  code->g = g;
  code->k = k;
  code->n = n;
  code->nn = nn;
  code->epsilon = epsilon;
}

/**
 * \fn void rbc_43_augmented_gabidulin_encode(rbc_43_qre c, const rbc_43_augmented_gabidulin gc, const rbc_43_vec m)
 * \brief This function encodes a message into a codeword.
 *
 * \param[out] c Vector of size <b>n</b> representing a codeword
 * \param[in] gc Augmented Gabidulin code
 * \param[in] m Vector of size <b>k</b> representing a message
 */
void rbc_43_augmented_gabidulin_encode(rbc_43_qre c, const rbc_43_augmented_gabidulin gc, const rbc_43_vec m)
{
  // Compute generator matrix
  rbc_43_elt matrix[gc.k][gc.n];
  for (size_t j = 0; j < gc.nn; ++j)
  {
    rbc_43_elt_set(matrix[0][j], gc.g[j]);
    for (size_t i = 1; i < gc.k; ++i)
    {
      rbc_43_elt_sqr(matrix[i][j], matrix[i - 1][j]);
    }
  }
  for (size_t i = 0; i < gc.k; ++i)
  {
    for (size_t j = gc.nn; j < gc.n; ++j)
    {
      rbc_43_elt_set_zero(matrix[i][j]);
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
  printf("\n\n\n# Augmented Gabidulin Encoding - Begin #");
  printf("\n\ng: ");
  rbc_43_vec_print(gc.g, gc.nn);

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
  printf("\n\n# Augmented Gabidulin Encoding - End #\n");
#endif
}

/**
 * \fn void rbc_43_augmented_gabidulin_encode_block(rbc_43_qre* c, const rbc_43_gabidulin gc, const rbc_43_vec m, size_t n1, size_t n2)
 * \brief This function encodes a message into a codeword, and then transforms it into a block.
 *
 * \param[out] c Vector of size <b>n1*n2</b> representing a codeword-block
 * \param[in] gc Augmented Gabidulin code
 * \param[in] m Vector of size <b>k</b> representing a message
 */
void rbc_43_augmented_gabidulin_encode_block(rbc_43_qre *c, const rbc_43_augmented_gabidulin gc, const rbc_43_vec m, size_t n1, size_t n2)
{
  // Compute generator matrix
  rbc_43_elt matrix[gc.k][gc.n];

  for (size_t j = 0; j < gc.nn; ++j)
  {
    rbc_43_elt_set(matrix[0][j], gc.g[j]);
    for (size_t i = 1; i < gc.k; ++i)
    {
      rbc_43_elt_sqr(matrix[i][j], matrix[i - 1][j]);
    }
  }
  for (size_t i = 0; i < gc.k; ++i)
  {
    for (size_t j = gc.nn; j < gc.n; ++j)
    {
      rbc_43_elt_set_zero(matrix[i][j]);
    }
  }

  // Encode message
  rbc_43_elt tmp;
  rbc_43_elt_set_zero(tmp);
  for (size_t i = 0; i < gc.k; ++i)
  {
    for (size_t j1 = 0; j1 < n1; j1++)
    {
      for (size_t j2 = 0; j2 < n2; j2++)
      {
        rbc_43_elt_mul(tmp, m[i], matrix[i][j1 * n2 + j2]);
        rbc_43_elt_add(c[j1]->v[j2], c[j1]->v[j2], tmp);
      }
    }
  }

#ifdef VERBOSE
  printf("\n\n\n# Augmented Gabidulin Encoding - Begin #");
  printf("\n\ng: ");
  rbc_43_vec_print(gc.g, gc.nn);

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
  printf("\n\n# Augmented Gabidulin Encoding - End #\n");
#endif
}

#ifdef TIME
/**
 * \fn decoding_time_t rbc_43_augmented_gabidulin_decode(rbc_43_vec m, const rbc_43_gabidulin gc, const rbc_43_vec y)
 * \brief This function decodes a word and return the execution time (in CPU cycle) of each part of the fundtion.
 *
 * The provided decoding algorithm works as follows (see \cite gabidulin:welch and \cite gabidulin:generalized for details):
 *   1. Recover the support erasure of dimension at least epsilon (or failure) and V2, and reduce the instance to a Gabidulin decoding.
 *   2. Find a solution (<b>W</b>, <b>N</b>) of the q-polynomial Reconstruction2(<b>y'</b>, <b>gc.g</b>, <b>gc.k</b>, (<b>gc.n</b> - <b>gc.k</b>)/2) problem using \cite gabidulin:generalized (section 4, algorithm 5) ;
 *   3. Find <b>R2</b> by computing <b>W \ (N.A) + I</b> (see "Polynomials with lower degree" improvement from \cite gabidulin:generalized, section 4.4.2) ;
 *   4. Find <b>f</b> by computing <b>V2 \ R2</b> ;
 *   5. Retrieve the message <b>m</b> as the k first coordinates of <b>f</b>.
 *
 * \param[out] m Vector of size <b>k</b> representing a message
 * \param[in] gc Augmented Gabidulin code
 * \param[in] y Vector of size <b>n</b> representing a word to decode
 * \return decoding_time_t the execution time of each part of the decoding algorithm
 */
decoding_time_t rbc_43_augmented_gabidulin_decode(rbc_43_vec m, const rbc_43_augmented_gabidulin gc, const rbc_43_vec y)
#else
/**
 * \fn void rbc_43_augmented_gabidulin_decode(rbc_43_vec m, const rbc_43_gabidulin gc, const rbc_43_vec y)
 * \brief This function decodes a word.
 *
 * The provided decoding algorithm works as follows (see \cite gabidulin:welch and \cite gabidulin:generalized for details):
 *   1. Recover the support erasure of dimension at least epsilon (or failure) and V2, and reduce the instance to a Gabidulin decoding.
 *   2. Find a solution (<b>W</b>, <b>N</b>) of the q-polynomial Reconstruction2(<b>y'</b>, <b>gc.g</b>, <b>gc.k</b>, (<b>gc.n</b> - <b>gc.k</b>)/2) problem using \cite gabidulin:generalized (section 4, algorithm 5) ;
 *   3. Find <b>R2</b> by computing <b>W \ (N.A) + I</b> (see "Polynomials with lower degree" improvement from \cite gabidulin:generalized, section 4.4.2) ;
 *   4. Find <b>f</b> by computing <b>V2 \ R2</b> ;
 *   5. Retrieve the message <b>m</b> as the k first coordinates of <b>f</b>.
 *
 * \param[out] m Vector of size <b>k</b> representing a message
 * \param[in] gc Augmented Gabidulin code
 * \param[in] y Vector of size <b>n</b> representing a word to decode
 */
void rbc_43_augmented_gabidulin_decode(rbc_43_vec m, const rbc_43_augmented_gabidulin gc, const rbc_43_vec y)
#endif
{
  /*
   *  Step 1: Recover the support from the last coordinates and compute annihilator
   */

#ifdef VALGRIND
  rbc_43_vec_poison(y, gc.n);
#endif
#ifdef TIME
  long long t0, t1, t2, t3, t4, t5, t6, t7;
  t0 = cpucycles();
#endif

  uint32_t t = (gc.nn - gc.k + gc.epsilon) / 2;
  int32_t max_degree_N = ((gc.nn - gc.k + gc.epsilon) % 2 == 0) * (gc.k + t - 1) + ((gc.nn - gc.k + gc.epsilon) % 2 == 1) * (gc.k + t);

  rbc_43_qpoly V2;
  rbc_43_vec yy;

  rbc_43_qpoly_init(&V2, gc.epsilon);
  rbc_43_vec_init(&yy, gc.n - gc.nn);

  for (int i = 0; i < gc.n - gc.nn; i++)
  {
    rbc_43_elt_set(yy[i], y[gc.nn + i]);
  }
  rbc_43_vec_gauss(yy, gc.n - gc.nn, 0, NULL, 0); // Non reduced row echelon form

#ifdef TIME
  // t1: Gauss
  t1 = cpucycles() - t0;
  t0 += t1;
#endif

  // Coordinates are secret (and poisoned) but degree is public
  rbc_43_qpoly_annihilator(V2, yy, gc.epsilon);

#ifdef TIME
  // t2: Annihilator
  t2 = cpucycles() - t0;
  t0 += t1;
#endif

  for (int i = 0; i < gc.nn; i++)
  {
    rbc_43_qpoly_evaluate2(yy[i], V2, y[i], gc.epsilon); // y' = V2(y);
  }

  /*
   *  Step 2: Solving the q-polynomial reconstruction2 problem with parameters nn and k+epsilon
   */

  rbc_43_qpoly A, I, N0, N1, V0, V1, N1_result, V1_result;

  // Allocate more coordinates than needed for set_mask function
  rbc_43_qpoly_init(&A, max_degree_N); // deg(A) <= gc.k + gc.epsilon
  rbc_43_qpoly_init(&I, t);            // deg(I) <= gc.k + gc.epsilon -1

  rbc_43_qpoly_init(&N0, max_degree_N + 1);
  rbc_43_qpoly_init(&N1, max_degree_N);
  rbc_43_qpoly_init(&V0, t - gc.epsilon);
  rbc_43_qpoly_init(&V1, t); // deg(V1) <= gc.t - gc.epsilon
  rbc_43_qpoly_init(&N1_result, max_degree_N);
  rbc_43_qpoly_init(&V1_result, t); // deg(V1_result) <= gc.t - gc.epsilon

  rbc_43_qpoly qtmp1, qtmp2, qtmp3, qtmp4, qtmp5;
  rbc_43_qpoly_init(&qtmp1, max_degree_N);
  rbc_43_qpoly_init(&qtmp2, max_degree_N + 1); // deg(N1^q) <= max_degree_N + 1
  rbc_43_qpoly_init(&qtmp3, max_degree_N);
  rbc_43_qpoly_init(&qtmp4, max_degree_N); // deg(V1) <= t - gc.epsilon
  rbc_43_qpoly_init(&qtmp5, t - gc.epsilon);

  rbc_43_vec u0, u1, rand;
  rbc_43_vec_init(&u0, gc.nn);
  rbc_43_vec_init(&u1, gc.nn);
  rbc_43_vec_init(&rand, gc.nn);
  random_source prng;
  random_source_init(&prng, RANDOM_SOURCE_PRNG);
  rbc_43_vec_set_random(&prng, rand, gc.nn);

  rbc_43_elt e1, e2, tmp1, tmp2, tmp3, tmp4;
  uint32_t status = 0;

#ifdef TIME
  // t3: Evaluation (+ qpoly initialization)
  t3 = cpucycles() - t0;
  t0 += t3;
#endif

  // Initialization step

  // A(g[i]) = 0 for 0 <= i <= k - 1
  // I(g[i]) = y[i] for 0 <= i <= k - 1
  rbc_43_qpoly_set_interpolate_vect_and_zero(A, I, gc.g, yy, gc.k + gc.epsilon);

#ifdef VALGRIND
  // A is public, I is secret
  rbc_43_qpoly_poison(I); // already poisoned execpt extra coordinate (optional)
#endif

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

  rbc_43_vec_set_zero(u0, gc.k + gc.epsilon);
  rbc_43_vec_set_zero(u1, gc.k + gc.epsilon);
  for (size_t i = gc.k + gc.epsilon; i < gc.nn; ++i)
  {
    rbc_43_qpoly_evaluate2(u0[i], A, gc.g[i], gc.k + gc.epsilon);

    rbc_43_qpoly_evaluate2(tmp1, I, gc.g[i], gc.k + gc.epsilon - 1);
    rbc_43_elt_add(u1[i], tmp1, yy[i]);

    status = status | !rbc_43_elt_is_zero(u1[i]);
  }

#ifdef VERBOSE
  printf("\n\n# Augmented Gabidulin Decoding - Begin #");
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

#ifdef TIME
  // t4: Loidreau - Initialisation
  t4 = cpucycles() - t0;
  t0 += t4;
#endif

  // Interpolation step
  for (size_t i = gc.k + gc.epsilon; i < gc.nn; ++i)
  {
    uint32_t bound1 = (i - gc.k - gc.epsilon) / 2;     // deg(N0) <= k + epsilon + bound1, deg(V1) <= bound1
    uint32_t bound2 = (i - gc.k - gc.epsilon + 1) / 2; // deg(V0) <= bound2, deg(N1)<= k + epsilon - 1 + bound2
    uint32_t next = i;
    uint32_t r = 1;

#ifdef VALGRIND
    // Poisoning interpolation index, and therefore q-degrees
    poison(&next, sizeof(uint32_t));
    poison(&r, sizeof(uint32_t));
#endif

    for (size_t k = i; k < gc.nn; ++k)
    {
      r &= rbc_43_elt_is_zero(u1[k]);
      next = r * (next + 1) + (1 - r) * next;
    }

    status = (status & (next != gc.nn));
    next = (next == gc.nn) * (next - 1) + (1 - (next == gc.nn)) * next;

    // Permutation of the coordinates of positions i and next
    for (size_t k = i; k < gc.nn; k++)
    {
      rbc_43_elt_set_mask1(tmp3, u0[k], tmp3, (k == next));
      rbc_43_elt_set_mask1(tmp4, u1[k], tmp4, (k == next));
    }

    rbc_43_elt_set(tmp1, u0[i]);
    rbc_43_elt_set_mask1(u0[i], tmp3, rand[i], status);
    rbc_43_elt_set_mask2(tmp3, rand[i], tmp1, status);

    rbc_43_elt_set(tmp2, u1[i]);
    rbc_43_elt_set_mask1(u1[i], tmp4, rand[i], status);
    rbc_43_elt_set_mask2(tmp4, rand[i], tmp2, status);

    for (size_t k = i; k < gc.nn; k++)
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

    rbc_43_qpoly_scalar_mul2(qtmp1, N1, e1, gc.k + gc.epsilon - 1 + bound2);
    rbc_43_qpoly_qexp2(qtmp2, N1, gc.k + gc.epsilon - 1 + bound2);

    rbc_43_qpoly_scalar_mul2(qtmp4, V1, e1, bound1);
    rbc_43_qpoly_qexp2(qtmp5, V1, bound1);

    rbc_43_qpoly_scalar_mul2(N1, N1, e2, gc.k + gc.epsilon - 1 + bound2);
    rbc_43_qpoly_add2(N1, N0, N1, gc.k + gc.epsilon + bound1, gc.k + gc.epsilon - 1 + bound2);

    rbc_43_qpoly_scalar_mul2(V1, V1, e2, bound1);
    rbc_43_qpoly_add2(V1, V0, V1, bound2, bound1);

    rbc_43_qpoly_add2(N0, qtmp1, qtmp2, gc.k + gc.epsilon - 1 + bound2, gc.k + gc.epsilon + bound2);
    rbc_43_qpoly_add2(V0, qtmp4, qtmp5, bound1, bound1 + 1);

    rbc_43_qpoly_set_mask(N1_result, N1, N1_result, gc.k + gc.epsilon + bound1, status);
    rbc_43_qpoly_set_mask(V1_result, V1, V1_result, bound2, status);

    // Update discrepancies
    for (size_t k = i + 1; k < gc.nn; ++k)
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

#ifdef TIME
  // t5: Loidreau - Interpolation
  t5 = cpucycles() - t0;
  t0 += t5;
#endif

  /*
   *  Step 3-4: Computing f
   */

  uint32_t cmp_deg_N1 = (rbc_43_qpoly_get_degree(N1_result) == -1); // 1 if true
  uint32_t cmp_deg_V1 = (rbc_43_qpoly_get_degree(V1_result) == 0);

  // If both are true then status = 0 otherwise status = 1
  status = 1 - (cmp_deg_N1 & cmp_deg_V1);

  rbc_43_qpoly_set_mask(qtmp1, N1_result, A, max_degree_N, status);

  rbc_43_qpoly_set_mask(V1_result, V1_result, V1, t, status);

#ifdef VALGRIND
  // Ensure poisoning inputs of Left Euclidean division algorithm
  rbc_43_qpoly_poison(qtmp1);
  rbc_43_qpoly_poison(V1_result);
#endif

  int32_t deg1, deg2;
  deg1 = rbc_43_qpoly_get_degree(qtmp1);
  deg2 = rbc_43_qpoly_get_degree(V1_result);

  rbc_43_qpoly_left_div2(qtmp3, qtmp4, qtmp1, V1_result, deg1, deg2);

#ifdef TIME
  // t6: First left euclidean division
  t6 = cpucycles() - t0;
  t0 += t6;
#endif

  // bound = max(gc.t-gc.epsilon, t)
  rbc_43_qpoly_set_mask(qtmp3, qtmp3, I, t, status);

  rbc_43_qpoly_left_div2(qtmp1, qtmp4, qtmp3, V2, gc.k + gc.epsilon - 1, gc.epsilon);

#ifdef TIME
  // t7: Second left euclidean division
  t7 = cpucycles() - t0;
#endif

  /*
   *  Step 5: Decoding the message as the value of the k first coordinates of f (qtmp1 variable)
   */
  rbc_43_vec_set(m, qtmp1->values, gc.k);

#ifdef VERBOSE
  printf("\nquotient: ");
  rbc_43_qpoly_print(qtmp1);
  printf("\nremainder: ");
  rbc_43_qpoly_print(qtmp2);
  printf("\nmu: ");
  rbc_43_vec_print(m, gc.k);
  printf("\n# Augmented Gabidulin Decoding - End #\n");
#endif

#ifdef VALGRIND
  rbc_43_qpoly_unpoison(I);

  rbc_43_qpoly_unpoison(N0);
  rbc_43_qpoly_unpoison(N1);
  rbc_43_qpoly_unpoison(V0);
  rbc_43_qpoly_unpoison(V1);
  rbc_43_qpoly_unpoison(V2);

  rbc_43_qpoly_unpoison(N1_result);
  rbc_43_qpoly_unpoison(V1_result);

  rbc_43_qpoly_unpoison(qtmp1);
  rbc_43_qpoly_unpoison(qtmp2);
  rbc_43_qpoly_unpoison(qtmp3);
  rbc_43_qpoly_unpoison(qtmp4);
  rbc_43_qpoly_unpoison(qtmp5);

  rbc_43_vec_unpoison(u0, gc.n);
  rbc_43_vec_unpoison(u1, gc.n);
  rbc_43_vec_unpoison(rand, gc.n);
  rbc_43_vec_unpoison(yy, gc.n - gc.nn);
#endif

  rbc_43_qpoly_clear(A);
  rbc_43_qpoly_clear(I);

  rbc_43_qpoly_clear(N0);
  rbc_43_qpoly_clear(N1);
  rbc_43_qpoly_clear(V0);
  rbc_43_qpoly_clear(V1);
  rbc_43_qpoly_clear(V2);

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
  rbc_43_vec_clear(yy);
  random_source_clear(&prng);

#ifdef TIME
  return (decoding_time_t){.gauss = t1, .annihilator = t2, .evaluation = t3, .loidreau1 = t4, .loidreau2 = t5, .leftdiv1 = t6, .leftdiv2 = t7};
#endif
}