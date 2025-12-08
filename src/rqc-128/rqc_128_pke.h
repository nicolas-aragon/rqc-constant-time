/** 
 * \file rqc.h
 * \brief Functions of the RQC_PKE IND-CPA scheme
 */

#ifndef RQC_128_PKE_H
#define RQC_128_PKE_H

#include "rbc_43_vec.h"
#include "rbc_43_qre.h"


void rqc_128_pke_keygen(uint8_t* pk, uint8_t* sk);
void rqc_128_pke_encrypt(rbc_43_qre* u, rbc_43_qre* v, const rbc_43_vec m, uint8_t* theta, const uint8_t* pk);
void rqc_128_pke_decrypt(rbc_43_vec m, const rbc_43_qre* u, const rbc_43_qre* v, const uint8_t* sk);

#endif

