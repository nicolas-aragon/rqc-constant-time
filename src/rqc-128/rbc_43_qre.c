/**
 * \file rbc_43_qre.c
 * \brief Implementation of rbc_43_qre.h
 */

#include "rbc_43_elt.h"
#include "rbc_43_qre.h"
#include "rbc_43_poly.h"
#include "rbc_43_vec.h"

static uint32_t RBC_43_QRE_DEG;
static int rbc_43_init_qre_modulus = 0;
static rbc_43_poly_sparse rbc_43_qre_modulus;
static rbc_43_poly rbc_43_qre_modulus_inv;




/**
 * \fn void rbc_43_qre_init_modulus(uint32_t degree) {
 * \brief This function init the quotient ring modulus corresponding to PARAM_N.
 *
 * \param[in] degree Degree of the modulus
 */
void rbc_43_qre_init_modulus(uint32_t degree) {
  if(rbc_43_init_qre_modulus == 0) {
    RBC_43_QRE_DEG = degree - 1;
    rbc_43_qre_modulus = rbc_43_qre_get_modulus(degree);
    rbc_43_poly_init(&rbc_43_qre_modulus_inv, RBC_43_QRE_DEG + 1);
    rbc_43_poly_set_zero(rbc_43_qre_modulus_inv, RBC_43_QRE_DEG + 1);
    for(uint32_t i = 0 ; i < rbc_43_qre_modulus->coeffs_nb ; i++) {
      rbc_43_elt_set_one(rbc_43_qre_modulus_inv->v[rbc_43_qre_modulus->coeffs[i]]);
    }
  }

  rbc_43_init_qre_modulus++;
}




/**
 * \fn void rbc_43_qre_clear_modulus(void) {
 * \brief This function clears the quotient ring modulus in use.
 */
void rbc_43_qre_clear_modulus(void) {
  rbc_43_init_qre_modulus--;

  if(rbc_43_init_qre_modulus == 0) {
    rbc_43_poly_sparse_clear(rbc_43_qre_modulus);
    rbc_43_poly_clear(rbc_43_qre_modulus_inv);
  }
}

/**
 * \fn rbc_43_poly_sparse rbc_43_qre_get_modulus(uint32_t degree) {
 * \brief This function return the sparse polynomial used as the quotient ring modulus for PARAM_N.
 *
 * \param[in] degree Degree of the modulus
 */
rbc_43_poly_sparse rbc_43_qre_get_modulus(uint32_t degree) {
  rbc_43_poly_sparse modulus = NULL;
  uint32_t* values;
  if(degree == 104) {
    values = (uint32_t*) malloc(5 * sizeof(uint32_t));
    values[0] = 0;
    values[1] = 1;
    values[2] = 3;
    values[3] = 4;
    values[4] = 104;
    rbc_43_poly_sparse_init(&modulus, 5, values);
    free(values);
  }
  else if(degree == 52) {
    values = (uint32_t*) malloc(3 * sizeof(uint32_t));
    values[0] = 0;
    values[1] = 3;
    values[2] = 52;
    rbc_43_poly_sparse_init(&modulus, 3, values);
    free(values);
  }
  else if(degree == 113) {
    values = (uint32_t*) malloc(3 * sizeof(uint32_t));
    values[0] = 0;
    values[1] = 9;
    values[2] = 113;
    rbc_43_poly_sparse_init(&modulus, 3, values);
    free(values);
  }

 return modulus;
}




/**
 * \fn void rbc_43_qre_init(rbc_43_qre* p)
 * \brief This function allocates the memory for a rbc_43_qre element.
 *
 * \param[out] p Pointer to the allocated rbc_43_qre
 */
void rbc_43_qre_init(rbc_43_qre* p) {
  rbc_43_poly_init(p, RBC_43_QRE_DEG);
}




/**
 * \fn void rbc_43_qre_clear(rbc_43_qre p)
 * \brief This function clears the memory allocated for a rbc_43_qre element.
 *
 * \param[in] p rbc_43_qre
 */
void rbc_43_qre_clear(rbc_43_qre p) {
  rbc_43_poly_clear(p);
}




/**
 * \fn void rbc_43_qre_set_zero(rbc_43_qre o)
 * \brief This functions sets a rbc_43_qre to zero.
 *
 * \param[in] o rbc_43_qre
 */
void rbc_43_qre_set_zero(rbc_43_qre o) {
  rbc_43_poly_set_zero(o, RBC_43_QRE_DEG);
}




/**
 * \fn void rbc_43_qre_set_random(random_source* ctx, rbc_43_qre o)
 * \brief This function sets a rbc_43_qre with random values using NIST seed expander.
 *
 * \param[out] ctx random source
 * \param[out] o rbc_43_qre
 */
void rbc_43_qre_set_random(random_source* ctx, rbc_43_qre o) {
  rbc_43_poly_set_random(ctx, o, RBC_43_QRE_DEG);
}




/**
 * \fn rbc_43_qre_set_random_from_xof(rbc_43_qre o, void (*xof)(uint8_t *, size_t, const uint8_t *, size_t), const uint8_t *xof_input, uint32_t xof_size)
 * \brief This function sets a rbc_43_qre with random values using XOF.
 *
 * \param[out] o rbc_43_qre
 * \param[in] xof XOF procedure
 * \param[in] xof_input XOF input byte array
 * \param[in] xof_size XOF input byte array length of xof_input
 */
void rbc_43_qre_set_random_from_xof(rbc_43_qre o,
                                 void (*xof)(uint8_t *, size_t, const uint8_t *, size_t),
                                 const uint8_t *xof_input,
                                 uint32_t xof_size) {
  rbc_43_poly_set_random_from_xof(o, RBC_43_QRE_DEG, xof, xof_input, xof_size);
}




/**
 * \fn void rbc_43_qre_set_random_full_rank(random_source* ctx, rbc_43_qre o)
 * \brief This function sets a rbc_43_qre with random values using the NIST seed expander. The returned rbc_43_qre has full rank.
 *
 * \param[out] ctx random source
 * \param[out] o rbc_43_qre
 */
void rbc_43_qre_set_random_full_rank(random_source* ctx, rbc_43_qre o) {
  rbc_43_poly_set_random_full_rank(ctx, o, RBC_43_QRE_DEG);
}




/**
 * \fn void rbc_43_qre_set_random_full_rank_with_one(random_source* ctx, rbc_43_qre o)
 * \brief This function sets a rbc_43_qre with random values using the NIST seed expander. The returned rbc_43_qre has full rank and contains one.
 *
 * \param[out] ctx random source
 * \param[out] o rbc_43_qre
 */
void rbc_43_qre_set_random_full_rank_with_one(random_source* ctx, rbc_43_qre o) {
  rbc_43_poly_set_random_full_rank_with_one(ctx, o, RBC_43_QRE_DEG);
}




/**
 * \fn void rbc_43_qre_set_random_from_support(random_source* ctx, rbc_43_qre o, const rbc_43_vspace support, uint32_t support_size)
 * \brief This function sets a rbc_43_qre with random values using the NIST seed expander. The support of the rbc_43_qre returned by this function is the one given in input.
 *
 * \param[out] ctx random source
 * \param[out] o rbc_43_qre
 * \param[in] support Support of <b>o</b>
 * \param[in] support_size Size of the support
 * \param[in] copy_flag If not 0, the support is copied into random coordinates of the resulting vector
 */
void rbc_43_qre_set_random_from_support(random_source* ctx, rbc_43_qre o, const rbc_43_vspace support, uint32_t support_size, uint8_t copy_flag) {
  rbc_43_poly_set_random_from_support(ctx, o, RBC_43_QRE_DEG, support, support_size, copy_flag);
}


/**
 * \fn uint8_t rbc_43_qre_is_equal_to(const rbc_43_qre p1, const rbc_43_qre p2)
 * \brief This function test if two rbc_43_qre are equal.
 *
 * \param[in] p1 rbc_43_qre
 * \param[in] p2 rbc_43_qre
 * \return 1 if the rbc_43_qre are equal, 0 otherwise
 */
uint8_t rbc_43_qre_is_equal_to(const rbc_43_qre p1, const rbc_43_qre p2) {
  return rbc_43_poly_is_equal_to(p1, p2);
}




/**
 * \fn void rbc_43_qre_add(rbc_43_qre o, const rbc_43_qre p1, const rbc_43_qre p2)
 * \brief This function adds two rbc_43_qre.
 *
 * \param[out] o Sum of <b>p1</b> and <b>p2</b>
 * \param[in] p1 rbc_43_qre
 * \param[in] p2 rbc_43_qre
 */
void rbc_43_qre_add(rbc_43_qre o, const rbc_43_qre p1, const rbc_43_qre p2) {
  rbc_43_poly_add(o, p1, p2);
}




/**
 * \fn void rbc_43_qre_mul(rbc_43_qre o, const rbc_43_qre p1, const rbc_43_qre p2)
 * \brief This function multiplies two rbc_43_qre.
 *
 * \param[out] o Product of <b>p1</b> and <b>p2</b>
 * \param[in] p1 rbc_43_qre
 * \param[in] p2 rbc_43_qre
 */
void rbc_43_qre_mul(rbc_43_qre o, const rbc_43_qre p1, const rbc_43_qre p2) {
  if(rbc_43_init_qre_modulus == 0) {
    printf("Call to rbc_43_qre_mul with uninitialized modulus\n");
    exit(1);
  }

  rbc_43_poly_mulmod_sparse(o, p1, p2, rbc_43_qre_modulus);
}




/**
 * \fn void rbc_43_qre_inv(rbc_43_qre o, const rbc_43_qre p)
 * \brief This function performs the extended euclidean algorithm to compute the inverse of p.
 *
 * \param[out] o Inverse of <b>e</b> modulo <b>modulus</b>
 * \param[in] p rbc_43_qre
 * \param[in] modulus Polynomial
 */
void rbc_43_qre_inv(rbc_43_qre o, const rbc_43_qre p) {
  if(rbc_43_init_qre_modulus == 0) {
    printf("Call to rbc_43_qre_inv with uninitialized modulus\n");
    exit(1);
  }

  rbc_43_poly_inv(o, p, rbc_43_qre_modulus_inv);
}




/**
 * \fn void rbc_43_qre_to_string(uint8_t* str, const rbc_43_qre p)
 * \brief This function parses a rbc_43_qre into a string.
 *
 * \param[out] str String
 * \param[in] e rbc_43_qre
 */
void rbc_43_qre_to_string(uint8_t* str, const rbc_43_qre p) {
  rbc_43_poly_to_string(str, p);
}




/**
 * \fn void rbc_43_qre_from_string(rbc_43_qre o, const uint8_t* str)
 * \brief This function parses a string into a rbc_43_qre.
 *
 * \param[out] o rbc_43_qre
 * \param[in] str String to parse
 */
void rbc_43_qre_from_string(rbc_43_qre o, const uint8_t* str) {
  rbc_43_poly_from_string(o, str);
}




/**
 * \fn void rbc_43_qre_print(const rbc_43_qre p)
 * \brief This function displays a rbc_43_qre.
 *
 * \param[in] p rbc_43_qre
 */
void rbc_43_qre_print(const rbc_43_qre p) {
  rbc_43_poly_print(p);
}




/**
 * \fn void rbc_43_poly_set_monial(rbc_43_qre o, uint32_t degree)
 * \brief This functions sets a polynomial to x^degree.
 *
 * \param[in] p Polynomial
 * \param[in] degree Degree of the monomial
 */
void rbc_43_qre_set_monomial(rbc_43_qre o, uint32_t degree) {
  rbc_43_poly_set_monomial(o, degree);
}


/**
 * \fn void rbc_43_qre_add_block(rbc_43_qre* o, const rbc_43_qre* p1, const rbc_43_qre* p2)
 * \brief This function adds two rbc_43_qre blocks.
 *
 * \param[out] o Sum of <b>p1</b> and <b>p2</b>
 * \param[in] p1 rbc_43_qre*
 * \param[in] p2 rbc_43_qre*
 */
void rbc_43_qre_add_block(rbc_43_qre* o, const rbc_43_qre* p1, const rbc_43_qre* p2, size_t size) {
  for(size_t i=0; i<size; i++){
    rbc_43_qre_add(o[i], p1[i], p2[i]);
  }
}


/**
 * \fn void rbc_43_qre_mul_block(rbc_43_qre* o, const rbc_43_qre p1, const rbc_43_qre* p2)
 * \brief This function multiplies two rbc_43_qre blocks.
 *
 * \param[out] o Product of <b>p1</b> and <b>p2</b>
 * \param[in] p1 rbc_43_qre
 * \param[in] p2 rbc_43_qre*
 */
void rbc_43_qre_mul_block(rbc_43_qre* o, const rbc_43_qre p1, const rbc_43_qre* p2, size_t size) {
  for(size_t i=0; i < size; i++){
    rbc_43_qre_mul(o[i], p1, p2[i]);
  }
}

void rbc_43_qre_set_random_2_syndrome(random_source* ctx, rbc_43_qre o1, rbc_43_qre o2, uint32_t size, uint32_t rank1, uint32_t rank2){
  rbc_43_vec support1, support2, intersection;
  uint8_t rank_is_null = 1, rank_intersection = 1;
  int i, j, k;
  rbc_43_vec_init(&intersection, 43); //maximum rank
  rbc_43_vec_init(&support1, rank1);
  rbc_43_vec_init(&support2, rank2);
  do
  {
    rank_is_null = 1;
    //generate two random supports
    rbc_43_vec_set_random_full_rank(ctx, support1, rank1);
    rbc_43_vec_set_random_full_rank(ctx, support2, rank2);

    //check if their are all independent
    rbc_43_vspace_intersection(intersection, support1, support2, rank1, rank2);
    rank_intersection = rbc_43_vec_get_rank(intersection, 43);
    rank_is_null &= (rank_intersection==0);

  } while (!rank_is_null);
  rbc_43_qre_set_random_from_support(ctx, o1, support1, rank1, 0);
  rbc_43_qre_set_random_from_support(ctx, o2, support2, rank2, 0);
  rbc_43_vec_clear(intersection);
}

void rbc_43_qre_set_random_block_3_syndrome(random_source* ctx, rbc_43_qre* o1, rbc_43_qre* o2, rbc_43_qre* o3, uint32_t size, uint32_t* ranks){
  rbc_43_vec support1, support2, support3, intersection;
  uint8_t rank_is_null = 1, rank_intersection = 1;
  int i, j, k;
  rbc_43_vec_init(&intersection, 43); //maximum rank
  rbc_43_vec_init(&support1, ranks[0]);
  rbc_43_vec_init(&support2, ranks[1]);
  rbc_43_vec_init(&support3, ranks[2]);
  do
  {
    rank_is_null = 1;
    //generate three random supports
    rbc_43_vec_set_random_full_rank(ctx, support1,ranks[0]);
    rbc_43_vec_set_random_full_rank_with_one(ctx, support2, ranks[1]);
    rbc_43_vec_set_random_full_rank(ctx, support3,ranks[2]);

    //check if their are all independent
    rbc_43_vspace_intersection(intersection, support1, support2, ranks[0], ranks[1]); 
    rank_intersection = rbc_43_vec_get_rank(intersection, 43);
    rank_is_null &= (rank_intersection==0);
    rbc_43_vspace_intersection(intersection, support1, support3, ranks[0], ranks[2]); 
    rank_intersection = rbc_43_vec_get_rank(intersection, 43);
    rank_is_null &= (rank_intersection==0);
    rbc_43_vspace_intersection(intersection, support2, support3, ranks[1], ranks[2]);
    rank_intersection = rbc_43_vec_get_rank(intersection, 43);
    rank_is_null &= (rank_intersection==0);

  } while (!rank_is_null);
  for(int i=0; i < size; i++){
    rbc_43_qre_set_random_from_support(ctx, o1[i], support1, ranks[0], 0);
    rbc_43_qre_set_random_from_support(ctx, o2[i], support2, ranks[1], 0);
    rbc_43_qre_set_random_from_support(ctx, o3[i], support3, ranks[2], 0);
  }
  rbc_43_vec_clear(intersection);
}


uint32_t rbc_43_qre_block_get_rank(const rbc_43_qre* v, uint32_t size1, uint32_t size2){
  rbc_43_vec vtmp;
  rbc_43_vec_init(&vtmp, size1*size2);
  for(int i = 0; i < size1; i++){
    for(int j = 0; j < size2; j++){
      rbc_43_elt_set(vtmp[i*size2+j], v[i]->v[j]);
    }
  }
  uint32_t rank = rbc_43_vec_get_rank(vtmp, size1*size2);
  rbc_43_vec_clear(vtmp);
  return rank;
}
