/**
 * \file rbc_43_qpoly.c
 * \brief Implementation of rbc_43_qpoly.h
 *
 * The rbc_43_qpolys are polynomials over a field of the form \f$ P(x) = \sum_{i=0}^{n} p_i \times x^{q^i} \f$ with \f$ q \f$ a rational prime.
 * See \cite ore:qpolynomials for a description of their main properties.
 *
 */

#include "rbc_43_vec.h"
#include "rbc_43_qpoly.h"

#define SUCCESS 0;            /**< Return status indicating that computation ended normally */
#define INVALID_PARAMETERS 1; /**< Return status indicating that invalid inputs have been used */

/**
 * \fn void rbc_43_qpoly_init(rbc_43_qpoly* p, int32_t max_degree)
 * \brief This function initializes a q_polynomial.
 *
 * \param[in] p q_polynomial to init
 * \param[in] max_degree The maximum degree of the q_polynomial
 */
void rbc_43_qpoly_init(rbc_43_qpoly *p, int32_t max_degree)
{
  *p = malloc(sizeof(rbc_43_qpoly_struct));

  rbc_43_vec_init(&((*p)->values), max_degree + 1);
  (*p)->max_degree = max_degree;
  (*p)->degree = -1;
}

/**
 * \fn void rbc_43_qpoly_clear(rbc_43_poly o)
 * \brief This function clears a q_polynomial.
 *
 * \param[in] p q_polynomial to clear
 */
void rbc_43_qpoly_clear(rbc_43_qpoly o)
{
  rbc_43_vec_clear(o->values);
  free(o);
}

/**
 * \fn void rbc_43_qpoly_update_degree(rbc_43_qpoly p, int32_t position)
 * \brief This function updates the degree of a rbc_43_qpoly starting from a given position.
 *
 * \param[out] p q_polynomial
 * \param[in] position Position to start from
 */
void rbc_43_qpoly_update_degree(rbc_43_qpoly p, int32_t position)
{
  for (int32_t i = position; i >= 0; --i)
  {
    // NON CONSTANT
    if (rbc_43_elt_is_zero(p->values[i]) == 0)
    {
      p->degree = i;
      return;
    }
  }

  p->degree = -1;
}

/**
 * \fn int8_t rbc_43_qpoly_get_degree(rbc_43_qpoly p);
 * \brief This function computes the q-degree of \f$ p \f$ in constant time.
 *
 * \param[in] p rbc_43_qpoly
 * \return degree equals to \f$\deg_q(p)\f$, even if the degree is negative
 */
int32_t rbc_43_qpoly_get_degree(rbc_43_qpoly p)
{
  int32_t degree = p->max_degree;
  int32_t degree_found = 0;
  for (int32_t i = p->max_degree; i >= 0; i--)
  {
    degree_found = degree_found | (rbc_43_elt_is_zero(p->values[i]) == 0);
    degree = (-(!degree_found) & (i - 1)) | (-(degree_found)&degree);
  }
  return degree;
}

int32_t rbc_43_qpoly_get_degree2(rbc_43_qpoly p, int32_t bound)
{
  int32_t degree = bound;
  int32_t degree_found = 0;
  for (int32_t i = bound; i >= 0; i--)
  {
    degree_found = degree_found | (rbc_43_elt_is_zero(p->values[i]) == 0);
    degree = (-(!degree_found) & (i - 1)) | (-(degree_found)&degree);
  }
  return degree;
}

/**
 * \fn int8_t rbc_43_qpoly_lc_and_degree(rbc_43_elt lc, rbc_43_qpoly p);
 * \brief This function computes both leading coefficient and degree of a q-polynomial.
 *
 * \param[out] lc rbc_43_elt such that \f$ lc = p.v[p.degree] \f$
 * \param[in] p rbc_43_qpoly
 * \return degree the real degree of p
 */
void rbc_43_qpoly_lc(rbc_43_elt lc, rbc_43_qpoly p)
{
  int32_t lc_found = 0;

  rbc_43_elt_set(lc, p->values[p->max_degree]);
  lc_found = lc_found | (rbc_43_elt_is_zero(p->values[p->max_degree]) == 0);
  for (int i = p->max_degree - 1; i >= 0; i--)
  {
    rbc_43_elt_set_mask1(lc, lc, p->values[i], lc_found);
    lc_found = lc_found | (rbc_43_elt_is_zero(p->values[i]) == 0);
  }
}

/**
 * \fn int8_t rbc_43_qpoly_lc_and_degree(rbc_43_elt lc, rbc_43_qpoly p);
 * \brief This function computes both leading coefficient and degree of a q-polynomial.
 *
 * \param[out] lc rbc_43_elt such that \f$ lc = p.v[p.degree] \f$
 * \param[in] p rbc_43_qpoly
 * \return degree the real degree of p
 */
int32_t rbc_43_qpoly_lc_and_degree(rbc_43_elt lc, rbc_43_qpoly p)
{
  int32_t lc_found = 0;
  int32_t degree = p->max_degree;

  rbc_43_elt_set(lc, p->values[p->max_degree]);
  lc_found = lc_found | (rbc_43_elt_is_zero(p->values[p->max_degree]) == 0);
  for (int i = p->max_degree - 1; i >= 0; i--)
  {
    rbc_43_elt_set_mask1(lc, lc, p->values[i], lc_found);
    lc_found = lc_found | (rbc_43_elt_is_zero(p->values[i]) == 0);
    degree = (-(!lc_found) & (i - 1)) | (-(lc_found)&degree);
  }

  return degree;
}

/**
 * \fn int8_t rbc_43_qpoly_set(rbc_43_qpoly o, const rbc_43_qpoly p)
 * \brief This function copies a q_polynomial into another one.
 *
 * \param[out] o q_polynomial
 * \param[in] p q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree</b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_set(rbc_43_qpoly o, const rbc_43_qpoly p)
{
  if (o->max_degree < p->max_degree)
  {
    return INVALID_PARAMETERS;
  }

  for (int32_t i = 0; i <= p->max_degree; ++i)
  {
    rbc_43_elt_set(o->values[i], p->values[i]);
  }
  /*
  for(int32_t i = p->max_degree + 1 ; i <= o->max_degree ; ++i) {
    rbc_43_elt_set_zero(o->values[i]);
  }*/

  o->degree = rbc_43_qpoly_get_degree2(o, p->max_degree);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_set_mask(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, uint32_t mask)
 * \brief This function copies either q_polynomial p1 or p2 into q_polynomial o depending on the mask value.
 *
 * \param[out] o q_polynomial
 * \param[in] p1 q_polynomial
 * \param[in] p2 q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p1.degree || o.max_degree < p2.degree</b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_set_mask(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, int32_t bound, uint32_t mask)
{
  if (o->max_degree < bound)
  {
    return INVALID_PARAMETERS;
  }

  rbc_43_elt tmp;

  for (int32_t i = 0; i <= bound; ++i)
  {
    rbc_43_elt_set_mask1(o->values[i], p1->values[i], p2->values[i], mask);
  }
  /*
  for(int32_t i = bound+1 ; i <= o->max_degree ; ++i) {
    rbc_43_elt_set_zero(o->values[i]);
  }*/

  o->degree = rbc_43_qpoly_get_degree2(o, bound);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_set_zero(rbc_43_qpoly o)
 * \brief This function sets a q_polynomial to zero.
 *
 * \param[out] o q_polynomial
 */
int8_t rbc_43_qpoly_set_zero(rbc_43_qpoly o)
{
  o->degree = -1;
  rbc_43_vec_set_zero(o->values, o->max_degree + 1);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_set_one(rbc_43_qpoly o)
 * \brief This function sets a q_polynomial to one.
 *
 * \param[out] o q_polynomial
 */
int8_t rbc_43_qpoly_set_one(rbc_43_qpoly o)
{
  o->degree = 0;
  rbc_43_elt_set_one(o->values[0]);

  for (int32_t i = 1; i < o->max_degree + 1; ++i)
  {
    rbc_43_elt_set_zero(o->values[i]);
  }

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_set_interpolate_vect_and_zero(rbc_43_qpoly o1, rbc_43_qpoly o2, const rbc_43_vec& p1, const rbc_43_vec& p2, int32_t size);
 * \brief This function computes interleaved interpolations such that \f$ o1(p1_i) = 0 \f$ and \f$ o2(p1_i) = p2_i \f$ for \f$ 0 \leq i \leq size - 1 \f$.
 *
 * \param[out] o1 rbc_43_qpoly such that \f$ o1(p1_i) = 0 \f$ with \f$ 0 \leq i \leq size - 1 \f$
 * \param[out] o2 rbc_43_qpoly such that \f$ o2(p1_i) = p2_i \f$ with \f$ 0 \leq i \leq size - 1 \f$
 * \param[in] p1 Vector over a finite field
 * \param[in] p2 Vector over a finite field
 * \param[in] size Size of the vectors
 * \return INVALID_PARAMETERS if <b>o1.max_degree < size</b> or <b>o2.max_degree < size - 1 </b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_set_interpolate_vect_and_zero(rbc_43_qpoly o1, rbc_43_qpoly o2, const rbc_43_vec p1, const rbc_43_vec p2, int32_t size)
{
  if (o1->max_degree < size || o2->max_degree < size - 1)
  {
    return INVALID_PARAMETERS;
  }
  rbc_43_elt tmp1, tmp2, tmp3;

  rbc_43_qpoly qtmp;
  rbc_43_qpoly_init(&qtmp, o1->max_degree);

  rbc_43_qpoly_set_one(o1);
  rbc_43_qpoly_set_zero(o2);
  rbc_43_qpoly_set_zero(qtmp);
  uint32_t o1_degree = 0, o2_degree = -1;
  for (int32_t i = 0; i < size; ++i)
  {
    rbc_43_qpoly_evaluate2(tmp1, o1, p1[i], o1_degree);
    rbc_43_qpoly_evaluate2(tmp2, o2, p1[i], (o2_degree >> 31 & 1) + o2_degree); // handle the case  deg = -1

    rbc_43_elt_inv(tmp3, tmp1);
    rbc_43_elt_add(tmp2, tmp2, p2[i]);
    rbc_43_elt_mul(tmp2, tmp2, tmp3);
    rbc_43_qpoly_scalar_mul2(qtmp, o1, tmp2, o1_degree);
    rbc_43_qpoly_add2(o2, qtmp, o2, o1_degree, o2_degree);
    rbc_43_qpoly_qexp2(qtmp, o1, o1_degree);
    rbc_43_qpoly_scalar_mul2(o1, o1, tmp1, o1_degree);
    rbc_43_qpoly_add2(o1, o1, qtmp, o1_degree, o1_degree + 1);
    o1_degree++;
    o2_degree++;
  }
  o1->degree = o1_degree;
  o2->degree = o2_degree;
  rbc_43_elt_set_zero(tmp3);

  rbc_43_qpoly_clear(qtmp);
  rbc_43_elt_set_zero(tmp1);
  rbc_43_elt_set_zero(tmp2);
  rbc_43_elt_set_zero(tmp3);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_annihilator(rbc_qpoly p, const rbc_vec v, uint32_t size)
 * \brief This function computes the annulator polynomial: \f$ p(v_i) = 0 \f$.
 *
 *
 * \param[out] p q_polynomial equal to the quotient of the division
 * \param[in] v n-dimensional vector
 * \param[in] size dimension
 * \return INVALID_PARAMETERS if <b>q.max_degree != size</b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_annihilator(rbc_43_qpoly p, const rbc_43_vec v, int32_t size)
{
  if (p->max_degree != size)
  {
    return INVALID_PARAMETERS;
  }

  rbc_43_elt p_at_vi;
  rbc_43_qpoly t;
  rbc_43_qpoly_init(&t, size);
  rbc_43_qpoly_set_zero(t);
  rbc_43_qpoly_set_one(p);

  for (int i = 0; i < size; ++i)
  {
    rbc_43_qpoly_evaluate2(p_at_vi, p, v[i], i); // p_i(v_i)
    rbc_43_qpoly_qexp2(t, p, i);                 // (p_i^q)(x)
    rbc_43_qpoly_scalar_mul2(p, p, p_at_vi, i);  // (p_i(v_i) * p_i)(x)
    rbc_43_qpoly_add2(p, p, t, i, i + 1);        // (p_i^q + p_i(v_i) * p_i)(x)
  }

  rbc_43_elt_set_zero(p_at_vi);
  rbc_43_qpoly_clear(t);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_is_zero(const rbc_43_qpoly p)
 * \brief This function tests if a q_polynomial is equal to zero.
 *
 * \param[in] p q_polynomial
 * \return returns 1 if <b>p</b> is equal to zero, 0 otherwise
 */
int8_t rbc_43_qpoly_is_zero(const rbc_43_qpoly p)
{
  if (p->degree == -1)
  {
    return 1;
  }

  return 0;
}

/**
 * \fn void rbc_43_qpoly_evaluate(rbc_43_elt o, const rbc_43_qpoly p, const rbc_43_elt e)
 * \brief This function evaluates a q_polynomial on a finite field element.
 *
 * \param[out] o Finite field element equal to \f$ p(e) \f$
 * \param[in] p q_polynomial
 * \param[in] e Finite field element
 */
void rbc_43_qpoly_evaluate(rbc_43_elt o, const rbc_43_qpoly p, const rbc_43_elt e)
{
  rbc_43_elt tmp1, tmp2;

  rbc_43_elt_set(tmp1, e);
  rbc_43_elt_mul(o, p->values[0], tmp1);

  for (int32_t i = 1; i <= p->max_degree; ++i)
  {
    rbc_43_elt_sqr(tmp1, tmp1);
    rbc_43_elt_mul(tmp2, p->values[i], tmp1);
    rbc_43_elt_add(o, o, tmp2);
  }
}

/**
 * \fn void rbc_43_qpoly_evaluate2(rbc_43_elt o, const rbc_43_qpoly p, const rbc_43_elt e, uint32_t p_degree)
 * \brief This function evaluates a q_polynomial on a finite field element for a known degree (or an upper bound).
 *
 * \param[out] o Finite field element equal to \f$ p(e) \f$
 * \param[in] p q_polynomial
 * \param[in] e Finite field element
 * \param[in] p_degree Integer greater or equal to p.degree
 */
void rbc_43_qpoly_evaluate2(rbc_43_elt o, const rbc_43_qpoly p, const rbc_43_elt e, uint32_t p_degree)
{
  rbc_43_elt tmp1, tmp2;

  rbc_43_elt_set(tmp1, e);
  rbc_43_elt_mul(o, p->values[0], tmp1);

  for (int32_t i = 1; i <= p_degree; ++i)
  {
    rbc_43_elt_sqr(tmp1, tmp1);
    rbc_43_elt_mul(tmp2, p->values[i], tmp1);
    rbc_43_elt_add(o, o, tmp2);
  }
}

/**
 * \fn int8_t rbc_43_qpoly_scalar_mul(rbc_43_qpoly o, const rbc_43_qpoly p, const rbc_43_elt e)
 * \brief This function computes the scalar product between a q_polynomial and a finite field element.
 *
 * \param[out] o q_polynomial equal to \f$ e \times p \f$
 * \param[in] p q_polynomial
 * \param[in] e Finite field element
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree</b>, SUCESS otherwise
 */
int8_t rbc_43_qpoly_scalar_mul(rbc_43_qpoly o, const rbc_43_qpoly p, const rbc_43_elt e)
{
  if (o->max_degree < p->max_degree)
  {
    return INVALID_PARAMETERS;
  }
  rbc_43_elt tmp;
  for (int32_t i = 0; i <= p->max_degree; ++i)
  {
    rbc_43_elt_mul(tmp, p->values[i], e);
    rbc_43_elt_set(o->values[i], tmp);
  }

  o->degree = rbc_43_qpoly_get_degree2(o, p->max_degree);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_scalar_mul2(rbc_43_qpoly o, const rbc_43_qpoly p, const rbc_43_elt e, uint32_t p_degree)
 * \brief This function computes the scalar product between a q_polynomial of known degree (or upper bound) and a finite field element.
 *
 * \param[out] o q_polynomial equal to \f$ e \times p \f$
 * \param[in] p q_polynomial
 * \param[in] e Finite field element
 * \param[in] p_degree Integer greater or equal to p.degree
 * \return INVALID_PARAMETERS if <b>o.max_degree < p.degree</b>, SUCESS otherwise
 */
int8_t rbc_43_qpoly_scalar_mul2(rbc_43_qpoly o, const rbc_43_qpoly p, const rbc_43_elt e, uint32_t p_degree)
{
  if (o->max_degree < p_degree)
  {
    return INVALID_PARAMETERS;
  }
  rbc_43_elt tmp;
  for (int32_t i = 0; i <= p_degree; ++i)
  {
    rbc_43_elt_mul(tmp, p->values[i], e);
    rbc_43_elt_set(o->values[i], tmp);
  }

  o->degree = rbc_43_qpoly_get_degree2(p, p_degree);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_qexp(rbc_43_qpoly o, const rbc_43_qpoly p)
 * \brief This function computes the qth power of a q_polynomial.
 *
 * \param[out] o q_polynomial equal to \f$ p^q \f$ where <b>q</b> is the characteristic of the field
 * \param[in] p q_polynomial
 * \param[in] p_degree integer
 * \return INVALID_PARAMETERS if <b>o.max_degree < p_degree + 1</b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_qexp(rbc_43_qpoly o, const rbc_43_qpoly p)
{
  int32_t p_degree = rbc_43_qpoly_get_degree(p);
  if (o->max_degree < p_degree + 1)
  {
    return INVALID_PARAMETERS;
  }

  rbc_43_elt tmp;
  rbc_43_elt_set_zero(o->values[0]);
  for (int32_t i = 0; i <= p_degree; ++i)
  {
    rbc_43_elt_sqr(tmp, p->values[i]);
    rbc_43_elt_set_mask1(o->values[(i + 1) % RBC_43_FIELD_M], tmp, o->values[0], i <= p_degree);
  }

  o->degree = (p_degree + 1) % RBC_43_FIELD_M;

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_qexp2(rbc_43_qpoly o, const rbc_43_qpoly p)
 * \brief This function computes the qth power of a q_polynomial.
 *
 * \param[out] o q_polynomial equal to \f$ p^q \f$ where <b>q</b> is the characteristic of the field
 * \param[in] p q_polynomial
 * \param[in] p_degree Integer greater or equal to p.degree
 * \return INVALID_PARAMETERS if <b>o.max_degree < p_degree + 1</b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_qexp2(rbc_43_qpoly o, const rbc_43_qpoly p, uint32_t p_degree)
{
  if (o->max_degree < p_degree + 1)
  {
    return INVALID_PARAMETERS;
  }
  rbc_43_elt tmp;
  rbc_43_elt_set_zero(o->values[0]);
  for (int32_t i = 1; i <= p_degree + 1; ++i)
  {
    rbc_43_elt_sqr(tmp, p->values[i - 1]);
    rbc_43_elt_set(o->values[i % RBC_43_FIELD_M], tmp);
  }

  o->degree = rbc_43_qpoly_get_degree2(o, p_degree + 1);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_add(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2)
 * \brief This function adds two q_polynomials.
 *
 * \param[out] o Sum of q_polynomials <b>p1</b> and <b>p2</b>
 * \param[in] p1 q_polynomial
 * \param[in] p2 q_polynomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < max(p1.degree, p2.degree)</b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_add(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, int32_t bound)
{
  if (o->max_degree < bound)
  {
    return INVALID_PARAMETERS;
  }

  for (int32_t i = 0; i <= bound; ++i)
  {
    rbc_43_elt_add(o->values[i], p1->values[i], p2->values[i]);
  }
  /*
  for(int32_t i = bound + 1 ; i <= o->max_degree ; ++i) {
    rbc_43_elt_set_zero(o->values[i]);
  }*/

  o->degree = rbc_43_qpoly_get_degree2(o, bound);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_add2(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2)
 * \brief This function adds two q_polynomials.
 *
 * \param[out] o Sum of q_polynomials <b>p1</b> and <b>p2</b>
 * \param[in] p1 q_polynomial
 * \param[in] p2 q_polynomial
 * \param[in] p1_degree Integer greater or equal to p1.degree
 * \param[in] p2_degree Integer greater or equal to p2.degree
 * \return INVALID_PARAMETERS if <b>o.max_degree < max(p1.degree, p2.degree)</b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_add2(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, uint32_t p1_degree, uint32_t p2_degree)
{
  if (o->max_degree + 1 < p1_degree + 1 || o->max_degree + 1 < p2_degree + 1)
  {
    return INVALID_PARAMETERS;
  }
  uint32_t mask = p1_degree + 1 > p2_degree + 1; // USAGE FOR KNOWN BOUNDS ONLY
  int32_t max_degree = mask * p1_degree + (1 - mask) * p2_degree;
  int32_t min_degree = (1 - mask) * p1_degree + mask * p2_degree;

  rbc_43_elt tmp1, tmp2, zero;
  rbc_43_elt_set_zero(zero);

  for (int32_t i = 0; i <= min_degree; i++)
  {
    rbc_43_elt_add(o->values[i], p1->values[i], p2->values[i]);
  }
  if (mask)
  {
    for (int i = min_degree + 1; i <= max_degree; i++)
    {
      rbc_43_elt_set(o->values[i], p1->values[i]);
    }
  }
  else
  {
    for (int i = min_degree + 1; i <= max_degree; i++)
    {
      rbc_43_elt_set(o->values[i], p2->values[i]);
    }
  }
  /*
  for(int i = max_degree+1 ; i <= o->max_degree ; i++){
    rbc_43_elt_set_zero(o->values[i]);
  }*/

  o->degree = rbc_43_qpoly_get_degree2(o, max_degree);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_mul2(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, uint32_t p1_degree, uint32_t p2_degree)
 * \brief This function computes the multiplication of two q_polynomials.
 *
 * Multiplication of two q_polynomials is non commutative and is defined using composition namely \f$ p \times q = p \circ q \f$.
 * Execution time of this function depends on p1_degree and p2_degree rather than p1->degree and p2->degree as in rbc_43_qpoly_mul.
 *
 * \param[out] o q_poynomial equal to \f$ p1 \circ p2 \f$
 * \param[in] p1 q_polynomial
 * \param[in] p2 q_polynomial
 * \param[in] p1_degree Integer greater or equal to p1.degree
 * \param[in] p2_degree Integer greater or equal to p2.degree
 * \return INVALID_PARAMETERS if <b>o.max_degree < p1.degree + p2.degree</b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_mul2(rbc_43_qpoly o, const rbc_43_qpoly p1, const rbc_43_qpoly p2, uint32_t p1_degree, uint32_t p2_degree)
{
  if (o->max_degree < p1_degree + p2_degree)
  {
    return INVALID_PARAMETERS;
  }

  rbc_43_qpoly_set_zero(o);
  rbc_43_elt tmp1, tmp2;

  for (uint32_t j = 0; j <= p2_degree; ++j)
  {
    rbc_43_elt_set(tmp1, p2->values[j]);
    rbc_43_elt_mul(tmp2, p1->values[0], tmp1);
    rbc_43_elt_add(o->values[j], o->values[j], tmp2);
    for (uint32_t i = 1; i <= p1_degree; ++i)
    {
      rbc_43_elt_sqr(tmp1, tmp1);
      rbc_43_elt_mul(tmp2, p1->values[i], tmp1);
      rbc_43_elt_add(o->values[(i + j) % RBC_43_FIELD_M], o->values[(i + j) % RBC_43_FIELD_M], tmp2);
    }
  }
  o->degree = rbc_43_qpoly_get_degree2(o, p1_degree + p2_degree);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_43_qpoly_mul_monomial(rbc_43_qpoly o, const rbc_43_qpoly p, const rbc_43_elt e, uint32_t index)
 * \brief This function computes the multiplication of a q_polynomial and a q-monomial.
 *
 * Multiplication of two q_polynomials is non commutative and is defined using composition namely \f$ p \times q = p \circ q \f$.
 *
 * \param[out] o q_poynomial equal to \f$ p \times e.X^[index] \f$
 * \param[in] p q_polynomial
 * \param[in] e coefficient of the monomial
 * \param[in] index degree of the monomial
 * \return INVALID_PARAMETERS if <b>o.max_degree < p1.degree + p2.degree</b>, SUCCESS otherwise
 */
int8_t rbc_43_qpoly_mul_monomial(rbc_43_qpoly o, rbc_43_qpoly p, rbc_43_elt e, int32_t index, int32_t flag)
{
  int32_t p_degree = rbc_43_qpoly_get_degree(p);

  rbc_43_elt tmp1, tmp2;
  rbc_43_qpoly_set_zero(o);

  rbc_43_elt_set_mask1(tmp1, e, o->values[0], flag);

  rbc_43_elt_mul(o->values[index], p->values[0], tmp1);
  for (int32_t i = 1; i <= p->max_degree; ++i)
  {
    rbc_43_elt_sqr(tmp1, tmp1);
    rbc_43_elt_mul(tmp2, tmp1, p->values[i]);
    rbc_43_elt_add(o->values[(i + index) % RBC_43_FIELD_M], o->values[(i + index) % RBC_43_FIELD_M], tmp2);
  }
  o->degree = flag * (p_degree + index) + (flag - 1);

  return SUCCESS;
}

/**
 * \fn int8_t rbc_qpoly_left_div(rbc_qpoly q, rbc_qpoly r, const rbc_qpoly a, const rbc_qpoly b)
 * \brief This function computes the left division of two q_polynomials using euclidean algorithm: \f$ a = b \times q + r \f$.
 *
 * \param[out] q q_polynomial equal to the quotient of the division
 * \param[out] r q_polynomial equal to the remainder of the division
 * \param[in] a q_polynomial to divide
 * \param[in] b divisor q_polynomial
 * \return INVALID_PARAMETERS if <b>q.max_degree < a.degree - b.degree</b> or <b>r.max_degree < b.degree - 1</b> or <b>b = 0</b>, SUCCESS otherwise
 */
void rbc_43_qpoly_left_div(rbc_43_qpoly q, rbc_43_qpoly r, rbc_43_qpoly a, rbc_43_qpoly b)
{
  rbc_43_qpoly add_to_r;
  rbc_43_elt ib_db, ifrob, lc_b, lc_r, q_add_elt;
  int32_t degree_a, degree_b, degree_r, s, jump;
  uint32_t flag;

  rbc_43_qpoly_init(&add_to_r, a->max_degree + b->max_degree);

  rbc_43_qpoly_set(r, a);
  rbc_43_qpoly_set_zero(q);
  degree_a = rbc_43_qpoly_get_degree(a);
  degree_b = rbc_43_qpoly_lc_and_degree(lc_b, b);
  // Compute inverse of lc_b
  rbc_43_elt_inv(ib_db, lc_b);
  s = degree_a - degree_b - a->max_degree;

  for (int32_t i = a->max_degree; i >= 0; i--)
  {
    degree_r = rbc_43_qpoly_lc_and_degree(lc_r, r);

    jump = degree_r - degree_b - i;
    flag = (((s >> 31) | (jump >> 31)) & 1) ^ 1;

    // Update quotient

    // Compute r_dr * b_db^(-1)
    rbc_43_elt_mul(ifrob, ib_db, lc_r);

    // Compute m^[-db]
    rbc_43_elt_ifrobenius2(ifrob, ifrob, b);

    // Add it to q if needed
    rbc_43_elt_add(q_add_elt, q->values[i], ifrob); // instead of sum qpoly, just add in the i-th value
    rbc_43_elt_set_mask1(q->values[i], q_add_elt, q->values[i], flag);

    // Update reminder
    // Compute B * (m^[-db]*X^[i])
    rbc_43_qpoly_mul_monomial(add_to_r, b, ifrob, i, flag);
    // Add it to r if needed
    rbc_43_qpoly_add(r, r, add_to_r, r->max_degree);

    s++;
  }
  q->degree = rbc_43_qpoly_get_degree(q);
  rbc_43_qpoly_clear(add_to_r);
}

/**
 * \fn int8_t rbc_qpoly_left_div(rbc_qpoly q, rbc_qpoly r, const rbc_qpoly a, const rbc_qpoly b, uint32_t capacity, uint32_t k)
 * \brief This function computes the left division of two q_polynomials using euclidean algorithm: \f$ a = b \times q + r \f$.
 *
 * This function is a variant of rbc_qpoly_left_div that uses degrees of a and b
 *
 * \param[out] q q_polynomial equal to the quotient of the division
 * \param[out] r q_polynomial equal to the remainder of the division
 * \param[in] a q_polynomial to divide
 * \param[in] b divisor q_polynomial
 * \param[in] b_degree
 * \param[in] k
 * \return INVALID_PARAMETERS if <b>q.max_degree < a.degree - b.degree</b> or <b>r.max_degree < b.degree - 1</b> or <b>b = 0</b>, SUCCESS otherwise
 */
void rbc_43_qpoly_left_div2(rbc_43_qpoly q, rbc_43_qpoly r, rbc_43_qpoly a, rbc_43_qpoly b, int32_t degree_a, int32_t degree_b)
{
  rbc_43_qpoly add_to_r;
  rbc_43_elt ib_db, ifrob, lc_b, lc_r, q_add_elt;
  int32_t degree_r, degree_add_r, s, jump;
  uint32_t flag;

  rbc_43_qpoly_init(&add_to_r, a->max_degree + b->max_degree);

  rbc_43_qpoly_set(r, a);
  rbc_43_qpoly_set_zero(q);
  rbc_43_qpoly_lc(lc_b, b);
  // Compute inverse of lc_b
  rbc_43_elt_inv(ib_db, lc_b);

  for (int32_t i = a->max_degree; i >= 0; i--)
  {
    degree_r = rbc_43_qpoly_lc_and_degree(lc_r, r);

    s = degree_a - degree_b - i;
    jump = degree_r - degree_b - i;
    flag = (((s >> 31) | (jump >> 31)) & 1) ^ 1;

    // Update quotient

    // Compute r_dr * b_db^(-1)
    rbc_43_elt_mul(ifrob, ib_db, lc_r);

    // Compute m^[-db]
    rbc_43_elt_ifrobenius2(ifrob, ifrob, b);

    // Add it to q if needed
    rbc_43_elt_add(q_add_elt, q->values[i], ifrob); // instead of sum qpoly, just add in the i-th value
    rbc_43_elt_set_mask1(q->values[i], q_add_elt, q->values[i], flag);

    // Update reminder
    // Compute B * (m^[-db]*X^[i])
    rbc_43_qpoly_mul_monomial(add_to_r, b, ifrob, i, flag);
    // Add it to r
    rbc_43_qpoly_add(r, r, add_to_r, r->max_degree);
  }
  q->degree = rbc_43_qpoly_get_degree(q);
  rbc_43_qpoly_clear(add_to_r);
}

/**
 * \fn void rbc_43_qpoly_print(const rbc_43_qpoly p)
 * \brief This function prints a q_polynomial.
 *
 * \param p q_polynomial
 */
void rbc_43_qpoly_print(const rbc_43_qpoly p)
{
  if (rbc_43_qpoly_is_zero(p) == 1)
  {
    printf("0");
  }
  else
  {
    for (int32_t i = p->degree; i > 0; --i)
    {
      rbc_43_elt_print(p->values[i]);
      printf("x^[%i] + ", i);
    }
    rbc_43_elt_print(p->values[0]);
    printf(" x^[0]");
  }
  printf("\n");
}
