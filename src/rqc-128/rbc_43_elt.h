/**
 * \file rbc_43_vec.h
 * \brief Interface for finite field elements
 */

#ifndef RBC_43_ELT_H
#define RBC_43_ELT_H

#include "rbc_43.h"
#include "random_source.h"
#include "rbc_43_qpoly.h"

void rbc_43_field_init(void);
void rbc_43_elt_set_zero(rbc_43_elt o);

void rbc_43_elt_set_one(rbc_43_elt o);

void rbc_43_elt_set(rbc_43_elt o, const rbc_43_elt e);

void rbc_43_elt_set_mask1(rbc_43_elt o, const rbc_43_elt e1, const rbc_43_elt e2, uint32_t mask);

void rbc_43_elt_set_mask2(rbc_43_elt o1, rbc_43_elt o2, const rbc_43_elt e, uint32_t mask);

void rbc_43_elt_set_from_uint64(rbc_43_elt o, const uint64_t* e);

void rbc_43_elt_set_random(random_source* ctx, rbc_43_elt o);
uint8_t rbc_43_elt_is_zero(const rbc_43_elt e);

uint8_t rbc_43_elt_is_equal_to(const rbc_43_elt e1, const rbc_43_elt e2);

uint8_t rbc_43_elt_is_greater_than(const rbc_43_elt e1, const rbc_43_elt e2);

int32_t rbc_43_elt_get_degree(const rbc_43_elt e);

uint8_t rbc_43_elt_get_coefficient(const rbc_43_elt e, uint32_t index);

void rbc_43_elt_set_coefficient_vartime(rbc_43_elt o, uint32_t index, uint8_t bit);

void rbc_43_elt_add(rbc_43_elt o, const rbc_43_elt e1, const rbc_43_elt e2);

void rbc_43_elt_mul(rbc_43_elt o, const rbc_43_elt e1, const rbc_43_elt e2);

void rbc_43_elt_inv(rbc_43_elt o, const rbc_43_elt e);

void rbc_43_elt_sqr(rbc_43_elt o, const rbc_43_elt e);

void rbc_43_elt_nth_root(rbc_43_elt o, const rbc_43_elt e, uint32_t n);

void rbc_43_elt_ifrobenius(rbc_43_elt o, const rbc_43_elt e, uint32_t n);

void rbc_43_elt_ifrobenius2(rbc_43_elt o, rbc_43_elt e, rbc_43_qpoly p);


void rbc_43_elt_reduce(rbc_43_elt o, const rbc_43_elt_ur e);

void rbc_43_elt_print(const rbc_43_elt e);

void rbc_43_elt_ur_set(rbc_43_elt_ur o, const rbc_43_elt_ur e);

void rbc_43_elt_ur_set_zero(rbc_43_elt_ur o);

void rbc_43_elt_ur_set_from_uint64(rbc_43_elt_ur o, const uint64_t* e);

void rbc_43_elt_ur_mul(rbc_43_elt_ur o, const rbc_43_elt e1, const rbc_43_elt e2);

void rbc_43_elt_ur_sqr(rbc_43_elt_ur o, const rbc_43_elt e);

void rbc_43_elt_to_string(uint8_t* str, const rbc_43_elt e);

void rbc_43_elt_from_string(rbc_43_elt e, const uint8_t* str);

void rbc_43_elt_ur_print(const rbc_43_elt_ur e);

#endif

