#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <getopt.h>
#include <gmp.h>
#include <time.h>

#include "ss.h"
#include "randstate.h"

#define OPTIONS "b:i:n:d:s:vh"

int main(int argc, char **argv) {

    uint64_t opt = 0;
    uint64_t bits = 256;
    uint64_t iters = 50;
    uint64_t seed = time(NULL);

    bool verbose_output = false;

    char *username = NULL;
    char *pub_file = "ss.pub";
    char *priv_file = "ss.priv";

    mpz_t pq, p, q, n, d;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'b': // SPECIFY bits.
            bits = strtoul(optarg, NULL, 10);

            break;
        case 'i': // SPECIFY iterations.
            iters = strtoul(optarg, NULL, 10);

            break;
        case 'n': // SPECIFY public key file.
            pub_file = optarg;

            break;
        case 'd': // SPECIFY private key file.
            priv_file = optarg;

            break;
        case 's': // SPECIFY seed.
            seed = strtoul(optarg, NULL, 10);

            break;
        case 'v': // ENABLE verbose output.
            verbose_output = true;

            break;
        case 'h': // DISPLAY program usage.
            printf("SYNOPSIS\n");
            printf("   Generates an SS public/private key pair.\n\n");
            printf("USAGE\n");
            printf("   ./keygen [OPTIONS]\n\n");
            printf("OPTIONS\n");
            printf("   -h              Display program help and usage.\n");
            printf("   -v              Display verbose program output.\n");
            printf("   -b bits         Minimum bits needed for public key n (default: 256).\n");
            printf(
                "   -i iterations   Miller-Rabin iterations for testing primes (default: 50).\n");
            printf("   -n pbfile       Public key file (default: ss.pub).\n");
            printf("   -d pvfile       Private key file (default: ss.priv).\n");
            printf("   -s seed         Random seed for testing.\n");

            break;
        }
    }

    // OPEN the public and private key files.

    FILE *pbfile = fopen(pub_file, "w");

    if (pbfile == NULL) {
        fprintf(stderr, "Error: Keygen could not access public file.\n");
        return 1;
    }

    FILE *pvfile = fopen(priv_file, "w");

    if (pvfile == NULL) {
        fprintf(stderr, "Error: Keygen could not access private file.\n");
        return 1;
    }

    // ENSURE private key file permissions are set to READ and WRITE.

    fchmod(fileno(pvfile), S_IRUSR | S_IWUSR);

    // INITIALIZE multiple-precision variables and random state.

    mpz_inits(pq, p, q, n, d, NULL);
    randstate_init(seed);

    // GENERATE public and private keys.

    ss_make_pub(p, q, n, bits, iters);
    ss_make_priv(d, pq, p, q);

    // GET the current user's name.

    username = getenv("USER");

    // WRITE the public and private key to their respective files.

    ss_write_pub(n, username, pbfile);
    ss_write_priv(pq, d, pvfile);

    // DO if verbose output is enabled.

    if (verbose_output) {
        printf("user = %s\n", username);
        gmp_fprintf(stdout, "p (%d bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_fprintf(stdout, "q (%d bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_fprintf(stdout, "n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_fprintf(stdout, "pq (%d bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_fprintf(stdout, "d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    // CLOSE files and CLEAR variables.

    fclose(pbfile);
    fclose(pvfile);

    randstate_clear();

    mpz_clears(pq, p, q, n, d, NULL);

    return 0;
}
