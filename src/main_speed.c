#include "rqc-128/rqc_128_parameters.h"
#include "rqc-128/rqc_128_parsing.h"
#include "rqc-128/rqc_128_pke.h"
#include "rqc-128/api.h"

#define NUM_OF_TESTS 10000

static unsigned long long cpucycles() {
  unsigned long long result;
  __asm__ volatile(".byte 15;.byte 49;shlq $32,%%rdx;orq %%rdx,%%rax" : "=a" (result) ::  "%rdx");
  return result;
}


int main(void) {
  unsigned char pk[RQC_128_PUBLIC_KEY_BYTES] = {0};
  unsigned char sk[RQC_128_SECRET_KEY_BYTES] = {0};
  uint8_t ct[RQC_128_CIPHERTEXT_BYTES] = {0};
  uint8_t ss0[RQC_128_SHARED_SECRET_BYTES] = {0};
  uint8_t ss1[RQC_128_SHARED_SECRET_BYTES] = {0};
  
  unsigned long long time_kg = 0, time_enc = 0, time_dec = 0;
  unsigned long long t0;
  // Performs whole RQC scheme
  for (int i = 0; i < NUM_OF_TESTS; i++) {

    t0 = cpucycles();
    rqc_128_kem_keygen(pk, sk);
    time_kg += cpucycles() - t0;
    
    t0 = cpucycles();
    rqc_128_kem_encaps(ct, ss0, pk);
    time_enc += cpucycles() - t0;

    t0 = cpucycles();
    rqc_128_kem_decaps(ss1, ct, sk);
    time_dec += cpucycles() - t0;
    
    for(int j=0; j < RQC_128_SHARED_SECRET_BYTES; j++){
      if(ss0[j] != ss1[j]){
        printf("Test n°%u: not the same secret shared on coordinate %u\n", i+1, j+1);
        return EXIT_FAILURE;
      }
    }
  }
  
  printf("KeyGen %lld\n", time_kg/NUM_OF_TESTS);
  printf("Encaps %lld\n", time_enc/NUM_OF_TESTS);
  printf("Decaps %lld\n", time_dec/NUM_OF_TESTS);
  printf("Total %lld\n", (time_kg + time_enc + time_dec)/NUM_OF_TESTS);

  return EXIT_SUCCESS;
} 