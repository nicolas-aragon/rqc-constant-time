#include "rqc-128/rbc_43_elt.h"
#include "rqc-128/rbc_43_qpoly.h"
#include "rqc-128/rbc_43_augmented_gabidulin.h"
#include "rqc-128/rbc_43_qre.h"
#include "rqc-128/rbc_43_vec.h"
#include "rqc-128/rqc_128_parameters.h"
#include "rqc-128/rqc_128_parsing.h"
#include "rqc-128/rqc_128_pke.h"
#include "rqc-128/api.h"

#include <stdio.h>
#include <stdlib.h>


int main(void) {
    unsigned char pk[RQC_128_PUBLIC_KEY_BYTES] = {0};
    unsigned char sk[RQC_128_SECRET_KEY_BYTES] = {0};
    uint8_t ct[RQC_128_CIPHERTEXT_BYTES] = {0};
    uint8_t ss0[RQC_128_SHARED_SECRET_BYTES] = {0};
    uint8_t ss1[RQC_128_SHARED_SECRET_BYTES] = {0};

    printf("Keygen: ");
    rqc_128_kem_keygen(pk, sk);
    printf("DONE\n");
    printf("Encaps: ");
    rqc_128_kem_encaps(ct, ss0, pk);
    printf("DONE\n");
    printf("Decaps: ");
    rqc_128_kem_decaps(ss1, ct, sk);
    printf("DONE\n");
    for(int j=0; j < RQC_128_SHARED_SECRET_BYTES; j++){
        if(ss0[j] != ss1[j]){
            printf("Error: Not identical secrets\n");
        return EXIT_FAILURE;
        }
    }
    printf("Shared secret:\n");
    for(int i=0; i < RQC_128_SHARED_SECRET_BYTES; i++) printf("%u", ss0[i]);
    printf("\n");
    
    return EXIT_SUCCESS;
} 