/** 
 * \file rbc_43_qpoly.h
 * \brief Functions to manipulate q-polynomials. 
 *
 * The rbc_43_qpolys are polynomials over a field of the form \f$ P(x) = \sum_{i=0}^{n} p_i \times x^{q^i} \f$ with \f$ q \f$ a rational prime.
 * See \cite ore:qpolynomials for a description of their main properties.
 *
 */

#ifndef RBC_43_QPOLY_H
#define RBC_43_QPOLY_H

#include "rbc_43_elt.h"
#include "rbc_43_poly.h"

/**
  * \typedef rbc_43_qpoly
  * \brief Structure of a q-polynomial
  */
typedef struct {
  rbc_43_vec values; /**< Coefficients of the q-polynomial */
  int32_t max_degree; /**< Maximum q-degree that the q-polynomial may have. This value never changes */
  int32_t degree; /**< Q-degree of the q-polynomial. This value is updated whenever necessary */
} rbc_43_qpoly_struct;
typedef rbc_43_qpoly_struct* rbc_43_qpoly;

void rbc_43_qpoly_init(rbc_43_qpoly* p, int32_t max_degree);

void rbc_43_qpoly_clear(rbc_43_qpoly o);

int32_t rbc_43_qpoly_get_degree(rbc_43_qpoly p);

int32_t rbc_43_qpoly_get_degree2(rbc_43_qpoly p, int32_t bound);

void rbc_43_qpoly_update_degree(rbc_43_qpoly p, int32_t position);

void rbc_43_qpoly_lc(rbc_43_elt lc, rbc_43_qpoly p);

int32_t rbc_43_qpoly_lc_and_degree(rbc_43_elt lc, rbc_43_qpoly p);

int8_t rbc_43_qpoly_set(rbc_43_qpoly o, const rbc_43_qpoly p);

int8_t rbc_43_qpoly_set_mask(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, int32_t bound, uint32_t mask);

int8_t rbc_43_qpoly_set_zero(rbc_43_qpoly o);

int8_t rbc_43_qpoly_set_one(rbc_43_qpoly o);

int8_t rbc_43_qpoly_set_interpolate_vect_and_zero(rbc_43_qpoly o1, rbc_43_qpoly o2, const rbc_43_vec p1, const rbc_43_vec p2, int32_t size);

int8_t rbc_43_qpoly_annihilator(rbc_43_qpoly p, const rbc_43_vec v, int32_t size);

int8_t rbc_43_qpoly_is_zero(const rbc_43_qpoly p);

void rbc_43_qpoly_evaluate(rbc_43_elt o, const rbc_43_qpoly p, const rbc_43_elt e);

void rbc_43_qpoly_evaluate2(rbc_43_elt o, const rbc_43_qpoly p, const rbc_43_elt e, uint32_t p_degree);

int8_t rbc_43_qpoly_scalar_mul(rbc_43_qpoly o, const rbc_43_qpoly p, const rbc_43_elt e);

int8_t rbc_43_qpoly_scalar_mul2(rbc_43_qpoly o, const rbc_43_qpoly p, const rbc_43_elt e, uint32_t p_degree);

int8_t rbc_43_qpoly_qexp(rbc_43_qpoly o, const rbc_43_qpoly p);

int8_t rbc_43_qpoly_qexp2(rbc_43_qpoly o, const rbc_43_qpoly p, uint32_t p_degree);

int8_t rbc_43_qpoly_add(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, int32_t bound);

int8_t rbc_43_qpoly_add2(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, uint32_t p1_degree, uint32_t p2_degree);

int8_t rbc_43_qpoly_mul(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2);

int8_t rbc_43_qpoly_mul2(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, uint32_t p1_degree, uint32_t p2_degree);

int8_t rbc_43_qpoly_mul_monomial(rbc_43_qpoly o, rbc_43_qpoly p, rbc_43_elt e, int32_t index, int32_t flag);

void rbc_43_qpoly_left_div(rbc_43_qpoly q, rbc_43_qpoly r,  rbc_43_qpoly a, rbc_43_qpoly b);

void rbc_43_qpoly_left_div2(rbc_43_qpoly q, rbc_43_qpoly r,  rbc_43_qpoly a, rbc_43_qpoly b, int32_t degree_a, int32_t degree_b);

void rbc_43_qpoly_print(const rbc_43_qpoly p);

#endif