#include "rbc_43_elt.h"
#include "rbc_43_qpoly.h"
#include "rbc_43_qre.h"
#include "rbc_43_vec.h"
#include <stdio.h>
#include <stdlib.h>

#include "valgrind/memcheck.h"

//rename the functions to be easier to use and read
#define poison VALGRIND_MAKE_MEM_UNDEFINED
#define unpoison VALGRIND_MAKE_MEM_DEFINED

//Fucntions to poisoned structures

void rbc_43_vec_poison(rbc_43_vec v, uint32_t size);
void rbc_43_vec_unpoison(rbc_43_vec v, uint32_t size);
void rbc_43_vec_size_poison(rbc_43_vec v, uint32_t size);
void rbc_43_vec_size_unpoison(rbc_43_vec v, uint32_t size);

void rbc_43_qpoly_degree_poison(rbc_43_qpoly p);
void rbc_43_qpoly_degree_unpoison(rbc_43_qpoly p);
void rbc_43_qpoly_poison(rbc_43_qpoly p);
void rbc_43_qpoly_unpoison(rbc_43_qpoly p);

void rbc_43_qre_degree_poison(rbc_43_qre e);
void rbc_43_qre_degree_poison(rbc_43_qre e);
void rbc_43_qre_poison(rbc_43_qre e);
void rbc_43_qre_unpoison(rbc_43_qre e);

void rbc_43_vspace_poison(rbc_43_vspace vs, uint32_t size);
void rbc_43_vspace_unpoison(rbc_43_vspace vs, uint32_t size);
