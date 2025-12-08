/**
 * \file augmented_gabidulin.h
 * \brief Functions to encode and decode messages using Gabidulin codes
 *
 * The decoding algorithm provided is based on q_polynomials reconstruction, see \cite gabidulin:welch and \cite gabidulin:generalized for details.
 *
 */

#ifndef RBC_43_AUGMENTED_GABIDULIN_H
#define RBC_43_AUGMENTED_GABIDULIN_H

#include "rbc_43_vec.h"
#include "rbc_43_qre.h"

/**
 * \typedef rbc_43_augmented_gabidulin
 * \brief Structure of an augmented gabidulin code
 */
typedef struct rbc_43_augmented_gabidulin
{
  rbc_43_vec g;     /**< Generator vector defining the code */
  uint32_t k;       /**< Size of vectors representing messages */
  uint32_t n;       /**< Size of vectors representing codewords */
  uint32_t nn;      /**< Size of vectors representing Gabidulin codewords */
  uint32_t epsilon; /**< Lower-bound of the dimension of the support erasure */
} rbc_43_augmented_gabidulin;

void rbc_43_augmented_gabidulin_init(rbc_43_augmented_gabidulin *code, const rbc_43_vec g, uint32_t k, uint32_t n, uint32_t nn, uint32_t epsilon);

void rbc_43_augmented_gabidulin_encode(rbc_43_qre c, const rbc_43_augmented_gabidulin gc, const rbc_43_vec m);

void rbc_43_augmented_gabidulin_encode_block(rbc_43_qre *c, const rbc_43_augmented_gabidulin gc, const rbc_43_vec m, size_t n1, size_t n2);

#ifndef TIME

void rbc_43_augmented_gabidulin_decode(rbc_43_vec m, const rbc_43_augmented_gabidulin gc, const rbc_43_vec y);

#else

/**
 * \typedef decoding_time_t
 * \brief Structure of repartion time of the augmented Gabidulin decoding algorithm
 */
typedef struct decoding_time_t
{
  long long gauss;
  long long annihilator;
  long long evaluation;
  long long loidreau1;
  long long loidreau2;
  long long leftdiv1;
  long long leftdiv2;
} decoding_time_t;

static unsigned long long cpucycles();

decoding_time_t rbc_43_augmented_gabidulin_decode(rbc_43_vec m, const rbc_43_augmented_gabidulin gc, const rbc_43_vec y);

#endif

#endif
