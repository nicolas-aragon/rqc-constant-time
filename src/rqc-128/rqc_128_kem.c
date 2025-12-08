/** 
 * \file kem.c
 * \brief Implementation of api.h
 */

#include "api.h"
#include "string.h"
#include "sha2.h"
#include "fips202.h"
#include "rqc_128_pke.h"
#include "rbc_43_qre.h"
#include "rqc_128_parameters.h"
#include "rqc_128_parsing.h"

/** 
 * \fn int rqc_128_kem_keygen(uint8_t* pk, uint8_t* sk)
 * \brief Keygen of the RQC_128_KEM IND-CCA2 scheme
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vectors <b>g</b> and <b>h</b>.
 *
 * The secret key is composed of the seed used to generate the vectors <b>x</b> and <b>y</b>.
 * As a technicality, the public key is appended to the secret key in order to respect the NIST API.
 *
 * \param[out] pk String containing the public key
 * \param[out] sk String containing the secret key
 * \return 0 if keygen is sucessfull
 */
int rqc_128_kem_keygen(uint8_t* pk, uint8_t* sk) {
  #ifdef VERBOSE
    printf("\n\n\n### KEYGEN ###");
  #endif
  
  rbc_43_field_init();
  rbc_43_qre_init_modulus(RQC_128_PARAM_N2);
  rqc_128_pke_keygen(pk, sk);
  rbc_43_qre_clear_modulus();
  return 0;
}



/** 
 * \fn int rqc_128_kem_encaps(uint8_t* ct, uint8_t* ss, const uint8_t* pk)
 * \brief Encapsulation of the RQC_128_KEM IND-CCA2 scheme
 *
 * \param[out] ct String containing the ciphertext
 * \param[out] ss String containing the shared secret
 * \param[in] pk String containing the public key
 * \return 0 if encapsulation is sucessfull
 */
int rqc_128_kem_encaps(uint8_t* ct, uint8_t* ss, const uint8_t* pk) {
  #ifdef VERBOSE
    printf("\n\n\n\n### ENCAPS ###");
  #endif

  uint8_t theta[SHA512_BYTES] = {0};
  uint8_t mc[RQC_128_VEC_K_BYTES + 2 * RQC_128_PARAM_N1 * RQC_128_VEC_N2_BYTES] = {0};
  uint8_t d[SHA512_BYTES] = {0};
  rbc_43_vec m;
  rbc_43_qre u[RQC_128_PARAM_N1], v[RQC_128_PARAM_N1];

  rbc_43_field_init();
  rbc_43_qre_init_modulus(RQC_128_PARAM_N2);
  rbc_43_vec_init(&m, RQC_128_PARAM_K);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_init(&u[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_init(&v[i]);
  random_source prng;
  random_source_init(&prng, RANDOM_SOURCE_PRNG);

  // Computing m
  rbc_43_vec_set_random(&prng, m, RQC_128_PARAM_K);

  // Computing theta
  rbc_43_vec_to_string(mc, m, RQC_128_PARAM_K);
  sha3_512(theta, mc, RQC_128_VEC_K_BYTES);
  
  // Encrypting m
  rqc_128_pke_encrypt(u, v, m, theta, pk);

  // Computing d
  sha512(d, mc, RQC_128_VEC_K_BYTES);
  // Computing ciphertext
  rqc_kem_ciphertext_to_string(ct, u, v, d);

  // Computing shared secret
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_to_string(mc + RQC_128_VEC_K_BYTES + i * RQC_128_VEC_N2_BYTES, u[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_to_string(mc + RQC_128_VEC_K_BYTES + (i+RQC_128_PARAM_N1) * RQC_128_VEC_N2_BYTES, v[i]);
  sha512(ss, mc, RQC_128_VEC_K_BYTES + 2 * RQC_128_PARAM_N1 * RQC_128_VEC_N2_BYTES);
  #ifdef VERBOSE
    printf("\n\nm: "); rbc_43_vec_print(m, RQC_128_PARAM_K);
    printf("\n\ntheta: "); for(int i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", theta[i]);
    printf("\n\nciphertext: "); for(int i = 0 ; i < RQC_128_CIPHERTEXT_BYTES ; ++i) printf("%02x", ct[i]);
    printf("\n\nd: "); for(int i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", d[i]);
    printf("\n\nsecret 1: "); for(int i = 0 ; i < RQC_128_SHARED_SECRET_BYTES ; ++i) printf("%02x", ss[i]);
  #endif
  
  rbc_43_vec_clear(m);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_clear(u[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_clear(v[i]);
  
  rbc_43_qre_clear_modulus();

  return 0;
}



/** 
 * \fn int rqc_128_kem_decaps(uint8_t* ss, const uint8_t* ct, const uint8_t* sk)
 * \brief Decapsulation of the RQC_128_KEM IND-CCA2 scheme
 *
 * \param[out] ss String containing the shared secret
 * \param[in] ct String containing the ciphertext
 * \param[in] sk String containing the secret key
 * \return 0 if decapsulation is successfull, -1 otherwise
 */
int rqc_128_kem_decaps(uint8_t* ss, const uint8_t* ct, const uint8_t* sk) {
  #ifdef VERBOSE
    printf("\n\n\n\n### DECAPS ###");
  #endif

  int8_t result = -1;
  uint8_t pk[RQC_128_PUBLIC_KEY_BYTES] = {0};
  uint8_t theta[SHA512_BYTES] = {0};
  uint8_t d[SHA512_BYTES] = {0};
  uint8_t d2[SHA512_BYTES] = {0};
  uint8_t mc[RQC_128_VEC_K_BYTES + 2 * RQC_128_PARAM_N1*RQC_128_VEC_N2_BYTES] = {0};
  rbc_43_vec m;
  rbc_43_qre u[RQC_128_PARAM_N1], v[RQC_128_PARAM_N1], u2[RQC_128_PARAM_N1], v2[RQC_128_PARAM_N1];

  rbc_43_field_init();
  rbc_43_qre_init_modulus(RQC_128_PARAM_N2);

  rbc_43_vec_init(&m, RQC_128_PARAM_K);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_init(&u[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_init(&u2[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_init(&v[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_init(&v2[i]);

  // Retrieving u, v and d from ciphertext
  rqc_kem_ciphertext_from_string(u, v, d, ct);

  // Retrieving pk from sk
  memcpy(pk, sk + SEEDEXPANDER_SEED_BYTES, RQC_128_PUBLIC_KEY_BYTES);

  // Decrypting
  rqc_128_pke_decrypt(m, u, v, sk);

  // Computing theta
  rbc_43_vec_to_string(mc, m, RQC_128_PARAM_K);
  sha3_512(theta, mc, RQC_128_VEC_K_BYTES);

  // Encrypting m'
  rqc_128_pke_encrypt(u2, v2, m, theta, pk);

  // Computing d'
  sha512(d2, mc, RQC_128_VEC_K_BYTES);

  // Computing shared secret
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_to_string(mc + RQC_128_VEC_K_BYTES + i*RQC_128_VEC_N2_BYTES, u[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_to_string(mc + RQC_128_VEC_K_BYTES + (i+RQC_128_PARAM_N1)*RQC_128_VEC_N2_BYTES, v[i]);
  sha512(ss, mc, RQC_128_VEC_K_BYTES + 2 * RQC_128_PARAM_N1 * RQC_128_VEC_N2_BYTES);

  // Abort if c != c' or d != d'
  result = rbc_43_qre_is_equal_to(u[0], u2[0]) == 1;
  for(int i=1; i < RQC_128_PARAM_N1; i++) result &= rbc_43_qre_is_equal_to(u[i], u2[i]) == 1;
  for(int i=0; i < RQC_128_PARAM_N1; i++) result &= rbc_43_qre_is_equal_to(v[i], v2[i]) == 1;
  result &= memcmp(d, d2, SHA512_BYTES) == 0;
  for(size_t i = 0 ; i < RQC_128_SHARED_SECRET_BYTES ; i++) ss[i] = result * ss[i];
  result--;

  #ifdef VERBOSE
    printf("\n\npk: "); for(int i = 0 ; i < RQC_128_PUBLIC_KEY_BYTES ; ++i) printf("%02x", pk[i]);
    printf("\n\nsk: "); for(int i = 0 ; i < RQC_128_SECRET_KEY_BYTES ; ++i) printf("%02x", sk[i]);
    printf("\n\nciphertext: "); for(int i = 0 ; i < RQC_128_CIPHERTEXT_BYTES ; ++i) printf("%02x", ct[i]);
    printf("\n\nm: "); rbc_43_vec_print(m, RQC_128_PARAM_K);
    printf("\n\ntheta: "); for(int i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", theta[i]);
    printf("\n\n\n# Checking Ciphertext- Begin #");
    printf("\n\nu2: "); rbc_43_qre_print(u2);
    printf("\n\nv2: "); rbc_43_qre_print(v2);
    printf("\n\nd2: "); for(int i = 0 ; i < SHA512_BYTES ; ++i) printf("%02x", d2[i]);
    printf("\n\n# Checking Ciphertext - End #\n");
  #endif

  rbc_43_vec_clear(m);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_clear(u[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_clear(u2[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_clear(v[i]);
  for(int i=0; i < RQC_128_PARAM_N1; i++) rbc_43_qre_clear(v2[i]);
  rbc_43_qre_clear_modulus();

  return result;
}

