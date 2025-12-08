/** 
 * \file parsing.c
 * \brief Implementation of parsing.h
 */

#include "string.h"
#include "rbc_43_vspace.h"
#include "rqc_128_parameters.h"
#include "rqc_128_parsing.h"
#include "seedexpander.h"


/** 
 * \fn void rqc_secret_key_to_string(uint8_t* sk, const uint8_t* seed, const uint8_t* pk)
 * \brief This function parses a secret key into a string
 *
 * The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
 * As a technicality, the public key is appended to the secret key in order to respect the NIST API.
 *
 * \param[out] sk String containing the secret key
 * \param[in] seed Seed used to generate the vectors x and y
 * \param[in] pk String containing the public key
 */
void rqc_secret_key_to_string(uint8_t* sk, const uint8_t* seed, const uint8_t* pk) {
  memcpy(sk, seed, SEEDEXPANDER_SEED_BYTES);
  memcpy(sk + SEEDEXPANDER_SEED_BYTES, pk, RQC_128_PUBLIC_KEY_BYTES);
}



/** 
* \fn void rqc_secret_key_from_string(rbc_43_qre x, rbc_43_qre y, uint8_t* pk, const uint8_t* sk)
* \brief This function parses a secret key from a string
*
* The secret key is composed of the seed used to generate vectors <b>x</b> and <b>y</b>.
* As a technicality, the public key is appended to the secret key in order to respect the NIST API.
*
* \param[out] x rbc_43_qre representation of vector x
* \param[out] y rbc_43_qre representation of vector y
* \param[out] pk String containing the public key
* \param[in] sk String containing the secret key
*/
void rqc_secret_key_from_string(rbc_43_qre x, rbc_43_qre y, uint8_t* pk, const uint8_t* sk) {
  uint8_t sk_seed[SEEDEXPANDER_SEED_BYTES] = {0};
  random_source sk_seedexpander;
  random_source_init(&sk_seedexpander, RANDOM_SOURCE_SEEDEXP);

  memcpy(sk_seed, sk, SEEDEXPANDER_SEED_BYTES);
  random_source_seed(&sk_seedexpander, sk_seed);

  rbc_43_qre_set_random_2_syndrome(&sk_seedexpander, x, y, RQC_128_PARAM_N2, RQC_128_PARAM_W_RX, RQC_128_PARAM_W_RY);

  memcpy(pk, sk + SEEDEXPANDER_SEED_BYTES, RQC_128_PUBLIC_KEY_BYTES);

  random_source_clear(&sk_seedexpander);
}



/** 
 * \fn void rqc_public_key_to_string(uint8_t* pk, const rbc_43_qre s, const uint8_t* seed)
 * \brief This function parses a public key into a string
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vectors <b>g</b> and <b>h</b>.
 *
 * \param[out] pk String containing the public key
 * \param[in] s rbc_43_qre representation of vector s
 * \param[in] seed Seed used to generate the public key
 */
void rqc_public_key_to_string(uint8_t* pk, const rbc_43_qre s, const uint8_t* seed) {
  rbc_43_qre_to_string(pk, s);
  memcpy(pk + RQC_128_VEC_N2_BYTES, seed, SEEDEXPANDER_SEED_BYTES);
}



/** 
 * \fn void rqc_public_key_from_string(rbc_43_qre g, rbc_43_qre h, rbc_43_qre s, const uint8_t* pk)
 * \brief This function parses a public key from a string
 *
 * The public key is composed of the syndrom <b>s</b> as well as the seed used to generate vectors <b>g</b> and <b>h</b>.
 *
 * \param[out] g rbc_43_vec representation of vector g
 * \param[out] h rbc_43_qre representation of vector h
 * \param[out] s rbc_43_qre representation of vector s
 * \param[in] pk String containing the public key
 */
void rqc_public_key_from_string(rbc_43_vec g, rbc_43_qre h, rbc_43_qre s, const uint8_t* pk) {
  uint8_t pk_seed[SEEDEXPANDER_SEED_BYTES] = {0};
  random_source pk_seedexpander;
  random_source_init(&pk_seedexpander, RANDOM_SOURCE_SEEDEXP);

  rbc_43_qre_from_string(s, pk);

  memcpy(pk_seed, pk + RQC_128_VEC_N2_BYTES, SEEDEXPANDER_SEED_BYTES);
  random_source_seed(&pk_seedexpander, pk_seed);

  rbc_43_vec_set_random_full_rank(&pk_seedexpander, g, RQC_128_PARAM_M);
  rbc_43_qre_set_random(&pk_seedexpander, h); 

  random_source_clear(&pk_seedexpander);
}



/** 
 * \fn void rqc_kem_ciphertext_to_string(uint8_t* ct, const rbc_43_qre u, const rbc_43_qre v, const uint8_t* d) {
 * \brief This function parses a ciphertext into a string
 *
 * The ciphertext is composed of vectors <b>u</b>, <b>v</b> and the hash <b>d</b>.
 *
 * \param[out] ct String containing the ciphertext
 * \param[in] u rbc_43_qre representation of vector u
 * \param[in] v rbc_43_qre representation of vector v
 * \param[in] d String containing the hash d
 */
void rqc_kem_ciphertext_to_string(uint8_t* ct, const rbc_43_qre* u, const rbc_43_qre* v, const uint8_t* d) {
  for(size_t i=0; i < RQC_128_PARAM_N1; i++){
    rbc_43_qre_to_string(ct + i * RQC_128_VEC_N2_BYTES, u[i]);
  }
  for(size_t i = 0; i < RQC_128_PARAM_N1; i++){
    rbc_43_qre_to_string(ct + (RQC_128_PARAM_N1+i)*RQC_128_VEC_N2_BYTES, v[i]);
  }
  memcpy(ct + 2 * RQC_128_PARAM_N1*RQC_128_VEC_N2_BYTES, d, SHA512_BYTES);
}



/** 
 * \fn void rqc_kem_ciphertext_from_string(rbc_43_qre u, rbc_43_qre v, uint8_t* d, const uint8_t* ct)
 * \brief This function parses a ciphertext from a string
 *
 * The ciphertext is composed of vectors <b>u</b>, <b>v</b> and the hash <b>d</b>.
 *
 * \param[out] u rbc_43_qre representation of vector u
 * \param[out] v rbc_43_qre representation of vector v
 * \param[out] d String containing the hash d
 * \param[in] ct String containing the ciphertext
 */
void rqc_kem_ciphertext_from_string(rbc_43_qre* u, rbc_43_qre* v, uint8_t* d, const uint8_t* ct) {
  for(size_t i=0; i < RQC_128_PARAM_N1; i++){
    rbc_43_qre_from_string(u[i], ct + i*RQC_128_VEC_N2_BYTES);
  }

  for(size_t i=0; i < RQC_128_PARAM_N1; i++){
    rbc_43_qre_from_string(v[i], ct + (RQC_128_PARAM_N1+i) * RQC_128_VEC_N2_BYTES);
  }
  memcpy(d, ct + 2 * RQC_128_PARAM_N1*RQC_128_VEC_N2_BYTES, SHA512_BYTES);
}

