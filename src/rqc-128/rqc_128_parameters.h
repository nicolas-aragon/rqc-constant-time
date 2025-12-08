/** 
 * \file parameters.h
 * \brief Parameters of the RQC_KEM MS-AG IND-CCA2 scheme
 */

#ifndef RQC_128_PARAMETER1_H
#define RQC_128_PARAMETER1_H

#define RQC_128_SECRET_KEY_BYTES 360 /**< Secret key size */
#define RQC_128_PUBLIC_KEY_BYTES 320 /**< Public key size */
#define RQC_128_SHARED_SECRET_BYTES 64 /**< Shared secret size */
#define RQC_128_CIPHERTEXT_BYTES 1184 /**< Ciphertext size */

#define RQC_128_PARAM_Q 2 /**< Parameter q of the scheme (finite field GF(q^m)) */
#define RQC_128_PARAM_M 43 /**< Parameter m of the scheme (finite field GF(q^m)) */
#define RQC_128_PARAM_K 3 /**< Parameter k of the scheme (code dimension) */
#define RQC_128_PARAM_N1 2 /**< Parameter n1 of the scheme (Fold size) */
#define RQC_128_PARAM_N2 52 /**< Parameter n2 of the scheme (Fold length) */
#define RQC_128_PARAM_N 104 /**< Parameter n2 of the scheme (Fold length) */
#define RQC_128_PARAM_EPSILON 32 /**< Parameter epsilon of the scheme (erasure dimension) */
#define RQC_128_PARAM_W_R1 4 /**< Parameter r_1 of the scheme (weight of vectors) */
#define RQC_128_PARAM_W_R2 4 /**< Parameter r_2 of the scheme (weight of vectors) */
#define RQC_128_PARAM_W_E 4 /**< Parameter r_2 of the scheme (weight of vectors) */
#define RQC_128_PARAM_W_RX 4 /**< Parameter r_x of the scheme (weight of vectors) */
#define RQC_128_PARAM_W_RY 4 /**< Parameter r_y of the scheme (weight of vectors) */
#define RQC_128_PARAM_W 36 /**< Parameter w of the scheme (decoding capacity of code) */
#define RQC_128_SECURITY 128 /**< Expected security level */

#define RQC_128_VEC_K_BYTES 17 /**< Number of bytes required to store a vector of size k */
#define RQC_128_VEC_N2_BYTES 280 /**< Number of bytes required to store a vector of size n2 */
#define SHA512_BYTES 64 /**< Size of SHA2_512 and SHA3_512 outputs */

#define SEEDEXPANDER_SEED_BYTES 40 /**< Seed size of the NIST seed expander */
#define SEEDEXPANDER_MAX_LENGTH 4294967295 /**< Max length of the NIST seed expander */

#endif

