#include <stdio.h>
#include <stdlib.h>

#include "mbedtls/include/polarssl/rsa.h"
#include "mbedtls/include/polarssl/entropy.h"
#include "mbedtls/include/polarssl/ctr_drbg.h"

#define ENC_SIZE 512
#define SECRETFILE "secret.encrypted"
#define NUMBER_OF_DECRYPTIONS 1

//#define VERBOSE


void hexprint(const unsigned char* buf, size_t buf_len) {
  for (size_t i = 0; i < buf_len; i++) {
    printf("%.2x", buf[i] & 0xff);
  }
}

int main(int argc, char* argv[]) {

  //
  // Initialize RNG
  //
#ifdef VERBOSE
  printf("[+] Initializing RNG...\n");
#endif
  entropy_context entropy;
  ctr_drbg_context ctr_drbg;
  entropy_init(&entropy);

  int ret;
  if ((ret = ctr_drbg_init(&ctr_drbg, entropy_func, &entropy,
                           NULL, 0)) != 0) {
    printf("[-] Error seeding the RNG\n");
    exit(1);
  }

  FILE* secret_fd;
  if ((secret_fd = fopen(SECRETFILE, "rb")) == NULL) {
    printf("[-] Error opening secret file\n");
    exit(1);
  }
  char secret_encrypted[ENC_SIZE] = {0};
  if (fread(secret_encrypted, 1, ENC_SIZE, secret_fd) != ENC_SIZE) {
    printf("[-] Error reading secret file\n");
    exit(1);
  }


  //
  // Read Private Key
  //
  rsa_context rsa;
  rsa_init(&rsa, RSA_PKCS_V15, 0);

  FILE* privkey_fd;
  if ((privkey_fd = fopen("rsa_priv.txt", "rb")) == NULL) {
    printf("[-] Error opening private key file\n");
    exit(1);
  }

  rsa_init(&rsa, RSA_PKCS_V15, 0);

  if ((ret = mpi_read_file(&rsa.N, 16, privkey_fd)) != 0 ||
      (ret = mpi_read_file(&rsa.E, 16, privkey_fd)) != 0 ||
      (ret = mpi_read_file(&rsa.D, 16, privkey_fd)) != 0 ||
      (ret = mpi_read_file(&rsa.P, 16, privkey_fd)) != 0 ||
      (ret = mpi_read_file(&rsa.Q, 16, privkey_fd)) != 0 ||
      (ret = mpi_read_file(&rsa.DP, 16, privkey_fd)) != 0 ||
      (ret = mpi_read_file(&rsa.DQ, 16, privkey_fd)) != 0 ||
      (ret = mpi_read_file(&rsa.QP, 16, privkey_fd)) != 0) {
    printf("Error reading private key file\n");
  }
  rsa.len = (mpi_msb(&rsa.N) + 7) >> 3;
  fclose(privkey_fd);

#ifdef VERBOSE
  char dq_ascii[4096 * 8] = {0};
  char dp_ascii[4096 * 8] = {0};
  size_t dq_ascii_len = 4096 * 8;
  size_t dp_ascii_len = 4096 * 8;
  ret = mpi_write_string(&rsa.DQ, 16, dq_ascii, &dq_ascii_len);
  if (ret) {
    printf("failed to decode DQ\n");
    exit(1);
  }
  ret = mpi_write_string(&rsa.DP, 16, dp_ascii, &dp_ascii_len);
  if (ret) {
    printf("failed to decode DP\n");
    exit(1);
  }
  printf("[+] Used DP: %s\n", dp_ascii);
  printf("[+] Used DQ: %s\n", dq_ascii);
#endif

  //
  // Decrypt Secret
  //
  unsigned char secret_decrypted[ENC_SIZE];
  size_t secret_decrypted_len;
  for (size_t i = 0; i < NUMBER_OF_DECRYPTIONS; i++) {
    // f_rng is NULL to disable blinding
    if ((ret = rsa_pkcs1_decrypt(&rsa, NULL, &ctr_drbg,
                                 RSA_PRIVATE, &secret_decrypted_len,
                                 (unsigned char*) secret_encrypted, secret_decrypted,
                                 ENC_SIZE)) != 0) {
      printf("[-] Error decrypting secret\n");
      exit(1);
    }
  }

#ifdef VERBOSE
  printf("Decrypted plaintext: %s\n", secret_decrypted);
  printf("[+] Finished\n");
#endif
  return 0;
}
