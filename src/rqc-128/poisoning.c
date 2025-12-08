#include "poisoning.h"


//POISON: on coefficients
void rbc_43_vec_poison(rbc_43_vec v, uint32_t size){
  for(int i=0; i<size; i++){
    poison((uint64_t) &v[i], sizeof(uint64_t));
  }
}

void rbc_43_vec_unpoison(rbc_43_vec v, uint32_t size){
  for(int i=0; i<size; i++){
    unpoison((uint64_t) &v[i], sizeof(uint64_t));
  }
}

//POISON: on coefficients
void rbc_43_vec_size_poison(rbc_43_vec v, uint32_t size){
  poison(&size, sizeof(uint32_t));
}

//POISON: on coefficients
void rbc_43_vec_size_unpoison(rbc_43_vec v, uint32_t size){
  unpoison(&size, sizeof(uint32_t));
}

//POISON: only on the degree
void rbc_43_qpoly_degree_poison(rbc_43_qpoly p){
  poison(&p->degree, sizeof(int32_t));
}

void rbc_43_qpoly_degree_unpoison(rbc_43_qpoly p){
  unpoison(&p->degree, sizeof(int32_t));
}

//POISON: on degree and coefficients
void rbc_43_qpoly_poison(rbc_43_qpoly p){
  poison(&p->degree, sizeof(int32_t));
  rbc_43_vec_poison(p->values, p->max_degree);
}

void rbc_43_qpoly_unpoison(rbc_43_qpoly p){
  unpoison(&p->degree, sizeof(int32_t));
  rbc_43_vec_unpoison(p->values, p->max_degree);
}

//POISON: on coefficients (only for constant polynomials, eg: V2)
void rbc_43_qpoly_coefficients_poison(rbc_43_qpoly p){
  rbc_43_vec_poison(p->values, p->max_degree);
}

void rbc_43_qpoly_coefficients_unpoison(rbc_43_qpoly p){
  rbc_43_vec_unpoison(p->values, p->max_degree);
}


//POISON: only on the degree
void rbc_43_qre_degree_poison(rbc_43_qre e){
  poison(&e->degree, sizeof(int32_t));
}

void rbc_43_qre_degree_unpoison(rbc_43_qre e){
  unpoison(&e->degree, sizeof(int32_t));
}

//POISON: on degree and coefficients
void rbc_43_qre_poison(rbc_43_qre e){
  poison(&e->degree, sizeof(int32_t));
  rbc_43_vec_poison(e->v, e->max_degree);
}

void rbc_43_qre_unpoison(rbc_43_qre e){
  unpoison(&e->degree, sizeof(int32_t));
  rbc_43_vec_unpoison(e->v, e->max_degree);
}

//POISON: on the size and the coefficients
void rbc_43_vspace_poison(rbc_43_vspace vs, uint32_t size){
  for(int i=0; i<size; i++){
    poison(&vs[i], sizeof(rbc_43_elt));
  }
  poison(&size, sizeof(uint32_t));
}

void rbc_43_vspace_unpoison(rbc_43_vspace vs, uint32_t size){
  unpoison(&size, sizeof(uint32_t));
  for(int i=0; i<size; i++){
    unpoison(vs[i], sizeof(vs[i]));
  }
}
