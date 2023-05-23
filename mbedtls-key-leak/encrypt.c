#include <stdio.h>
#include <stdlib.h>

#include "mbedtls/include/polarssl/rsa.h"
#include "mbedtls/include/polarssl/entropy.h"
#include "mbedtls/include/polarssl/ctr_drbg.h"

#define ENC_SIZE 512
#define SECRETFILE "secret.encrypted"

void hexprint(const unsigned char* buf, size_t buf_len) {
  for (size_t i = 0; i < buf_len; i++) {
    printf("%.2x", buf[i] & 0xff);
  }
}

int main(int argc, char* argv[]) {

  //
  // Initialize RNG
  //
  //printf("[+] Initializing RNG...\n");
  entropy_context entropy;
  ctr_drbg_context ctr_drbg;
  entropy_init(&entropy);
  char secret[1024] = "CR4ck_M3_1F_V_C4n!";
  if (argc == 2) {
    strncpy(secret, argv[1], 1024);
  }

  if ((ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
                           NULL, 0)) != 0) {
    printf("[-] Error when seeding the RNG\n");
    exit(1);
  }

  //
  // Read Public Key
  //
  //printf("[+] Reading public key...\n");
  rsa_context rsa;
  rsa_init(&rsa, RSA_PKCS_V15, 0);
  FILE* fd;
  if ((fd = fopen("rsa_pub.txt", "rb")) == NULL) {
    printf("[-] Error opening public key\n");
    exit(1);
  }

  if ((mpi_read_file(&rsa.N, 16, fd)) != 0 ||
      (mpi_read_file(&rsa.E, 16, fd)) != 0) {
    printf("Error reading public key file\n");
    exit(1);
  }
  rsa.len = (mpi_msb(&rsa.N) + 7) >> 3;
  fclose(fd);

  //
  // Encrypt Secret
  //
  //printf("[+] Encrypting...\n");
  unsigned char secret_encrypted[ENC_SIZE];
  if ((rsa_pkcs1_encrypt(&rsa, ctr_drbg_random, &ctr_drbg,
                               RSA_PUBLIC, strlen(secret),
                               (const unsigned char*) secret, secret_encrypted)) != 0) {
    printf("[-] Error encrypting secret\n");
  }
  FILE* out_fd;
  if ((out_fd = fopen(SECRETFILE, "w")) == NULL) {
    printf("[-] Error opening output file\n");
    exit(1);
  }
  if (fwrite(secret_encrypted, 1, ENC_SIZE, out_fd) != ENC_SIZE) {
    printf("[-] Error storing encrypted secret\n");
    exit(1);
  }
  //printf("Ciphertext: ");
  //hexprint(secret_encrypted, ENC_SIZE);
  //printf("\n");

  //printf("[+] Finished\n");
  return 0;
}
