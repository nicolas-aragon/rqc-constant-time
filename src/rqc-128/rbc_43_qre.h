/**
 * \file rbc_43_qre.h
 * \brief Interface for quotient ring elements 
 * */

#ifndef RBC_43_QRE_H
#define RBC_43_QRE_H

#include "rbc_43.h"
#include "random_source.h"

void rbc_43_qre_init_modulus(uint32_t degree);

void rbc_43_qre_clear_modulus(void);

rbc_43_poly_sparse rbc_43_qre_get_modulus(uint32_t degree);

void rbc_43_qre_init(rbc_43_qre* p);

void rbc_43_qre_clear(rbc_43_qre p);

void rbc_43_qre_set_zero(rbc_43_qre o);

void rbc_43_qre_set_random(random_source* ctx, rbc_43_qre o);

void rbc_43_qre_set_random_from_xof(rbc_43_qre o,
                                 void (*xof)(uint8_t *, size_t, const uint8_t *, size_t),
                                 const uint8_t *xof_input,
                                 uint32_t xof_size);

void rbc_43_qre_set_random_full_rank(random_source* ctx, rbc_43_qre o);

void rbc_43_qre_set_random_full_rank_with_one(random_source* ctx, rbc_43_qre o);

void rbc_43_qre_set_random_from_support(random_source* ctx, rbc_43_qre o, const rbc_43_vspace support, uint32_t support_size, uint8_t copy_flag);

uint8_t rbc_43_qre_is_equal_to(const rbc_43_qre p1, const rbc_43_qre p2);

void rbc_43_qre_add(rbc_43_qre o, const rbc_43_qre p1, const rbc_43_qre p2);

void rbc_43_qre_mul(rbc_43_qre o, const rbc_43_qre p1, const rbc_43_qre p2);

void rbc_43_qre_inv(rbc_43_qre o, const rbc_43_qre p);

void rbc_43_qre_to_string(uint8_t* str, const rbc_43_qre p);

void rbc_43_qre_from_string(rbc_43_qre o, const uint8_t* str);

void rbc_43_qre_print(const rbc_43_qre p);

void rbc_43_qre_set_monomial(rbc_43_qre o, uint32_t degree);

void rbc_43_qre_rot(rbc_43_qre o, rbc_43_qre e, uint32_t rot); //?

void rbc_43_qre_add_block(rbc_43_qre* o, const rbc_43_qre* p1, const rbc_43_qre* p2, size_t size);

void rbc_43_qre_mul_block(rbc_43_qre* o, const rbc_43_qre p1, const rbc_43_qre* p2, size_t size);

uint32_t rbc_43_qre_block_get_rank(const rbc_43_qre* v, uint32_t size1, uint32_t size2);

void rbc_43_qre_set_random_2_syndrome(random_source* ctx, rbc_43_qre o1, rbc_43_qre o2, uint32_t size, uint32_t rank1, uint32_t rank2);

void rbc_43_qre_set_random_block_3_syndrome(random_source* ctx, rbc_43_qre* o1, rbc_43_qre* o2, rbc_43_qre* o3, uint32_t size, uint32_t* ranks);

#endif

