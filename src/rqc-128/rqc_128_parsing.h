/** 
 * \file parsing.h
 * \brief Functions to parse secret key, public key and ciphertext of the RQC scheme
 */

#ifndef RQC_128_PARSING_H
#define RQC_128_PARSING_H

#include "rbc_43_qre.h"
#include "rbc_43_vec.h"


void rqc_secret_key_to_string(uint8_t* sk, const uint8_t* seed, const uint8_t* pk);
void rqc_secret_key_from_string(rbc_43_qre x, rbc_43_qre y, uint8_t* pk, const uint8_t* sk);

void rqc_public_key_to_string(uint8_t* pk, const rbc_43_qre s, const uint8_t* seed);
void rqc_public_key_from_string(rbc_43_vec g, rbc_43_qre h, rbc_43_qre s, const uint8_t* pk);

void rqc_kem_ciphertext_to_string(uint8_t* ct, const rbc_43_qre* u, const rbc_43_qre* v, const uint8_t* d);
void rqc_kem_ciphertext_from_string(rbc_43_qre* u, rbc_43_qre* v, uint8_t* d, const uint8_t* ct);

#endif

