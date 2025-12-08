/**
 * \file api.h
 * \brief NIST KEM API used by the RQC_KEM IND-CCA2 scheme
 */


#ifndef RQC_API_H
#define RQC_API_H

#include <stdint.h>

int rqc_128_kem_keygen(uint8_t* pk, uint8_t* sk);
int rqc_128_kem_encaps(uint8_t* ct, uint8_t* ss, const uint8_t* pk);
int rqc_128_kem_decaps(uint8_t* ss, const uint8_t* ct, const uint8_t* sk);

#endif

