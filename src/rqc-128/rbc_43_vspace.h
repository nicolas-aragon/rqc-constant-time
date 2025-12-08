/**
 * \file rbc_43_vspace.h
 * \brief Interface for subspaces of Fq^m
 */

#ifndef RBC_43_VSPACE_H
#define RBC_43_VSPACE_H

#include "rbc_43.h"
#include "random_source.h"
#include "seedexpander_shake.h"

void rbc_43_vspace_init(rbc_43_vspace* vs, uint32_t size);

void rbc_43_vspace_clear(rbc_43_vspace vs);

void rbc_43_vspace_set(rbc_43_vspace o, const rbc_43_vspace vs, uint32_t size);

void rbc_43_vspace_set_zero(rbc_43_vspace o, uint32_t size);

void rbc_43_vspace_set_random_full_rank(random_source* ctx, rbc_43_vspace o, uint32_t size);

void rbc_43_vspace_set_random_full_rank_with_one(random_source* ctx, rbc_43_vspace o, uint32_t size);

void rbc_43_vspace_set_random_full_rank_with_one_tmp(seedexpander_shake_t* ctx, rbc_43_vspace o, uint32_t size);

void rbc_43_vspace_direct_sum(rbc_43_vspace o, const rbc_43_vspace vs1, const rbc_43_vspace vs2, uint32_t vs1_size, uint32_t vs2_size);

uint32_t rbc_43_vspace_intersection(rbc_43_vspace o, const rbc_43_vspace vs1, const rbc_43_vspace vs2, uint32_t vs1_size, uint32_t vs2_size);

void rbc_43_vspace_product(rbc_43_vspace o, const rbc_43_vspace vs1, const rbc_43_vspace vs2, uint32_t vs1_size, uint32_t vs2_size);

void rbc_43_vspace_print(const rbc_43_vspace vs, uint32_t size);

#endif

