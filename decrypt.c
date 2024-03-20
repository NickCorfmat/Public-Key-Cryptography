#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <getopt.h>
#include <gmp.h>
#include <time.h>

#include "ss.h"
#include "randstate.h"

#define OPTIONS "i:o:n:vh"

int main(int argc, char **argv) {

    uint64_t opt = 0;

    bool toggle_i = false;
    bool toggle_o = false;
    bool verbose_output = false;

    char *priv_file = "ss.priv";
    char *in_name = "default_input";
    char *out_name = "default_output";

    mpz_t pq, d;

    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
        switch (opt) {
        case 'i': // SPECIFY input file.
            toggle_i = true;
            in_name = optarg;

            break;
        case 'o': // SPECIFY output file.
            toggle_o = true;
            out_name = optarg;

            break;
        case 'n': // SPECIFY private key file.
            priv_file = optarg;

            break;
        case 'v': // ENABLE verbose output.
            verbose_output = true;

            break;
        case 'h': // DISPLAY program usage.
            printf("SYNOPSIS\n");
            printf("   Decrypts data using SS decryption.\n");
            printf("   Encrypted data is encrypted by the encrypt program.\n\n");
            printf("USAGE\n");
            printf("   ./decrypt [OPTIONS]\n\n");
            printf("OPTIONS\n");
            printf("   -h              Display program help and usage.\n");
            printf("   -v              Display verbose program output.\n");
            printf("   -i infile       Input file of data to decrypt (default: stdin).\n");
            printf("   -o outfile      Output file for decrypted data (default: stdout).\n");
            printf("   -n pvfile       Private key file (default: ss.priv).\n");

            break;
        }
    }

    // OPEN the private key file.

    FILE *pvfile = fopen(priv_file, "r");

    if (pvfile == NULL) {
        fprintf(stderr, "Error: Decrypt could not access private key file.\n");
        return 1;
    }

    // CREATE and OPEN input and output files.

    FILE *input_file;
    FILE *output_file;

    if (toggle_i) {
        input_file = fopen(in_name, "r");
        if (input_file == NULL) {
            fprintf(stderr, "Error: Decrypt could not access input file.\n");
            return 1;
        }
    } else {
        input_file = stdin;
    }

    if (toggle_o) {
        output_file = fopen(out_name, "w");
        if (output_file == NULL) {
            fprintf(stderr, "Error: Decrypt could not access output file.\n");
            return 1;
        }
    } else {
        output_file = stdout;
    }

    // INITIALIZE multiple-precision variables.

    mpz_inits(pq, d, NULL);

    // READ the private key from the private key file.

    ss_read_priv(pq, d, pvfile);

    // DO if verbose output is enabled.

    if (verbose_output) {
        gmp_fprintf(stdout, "pq (%d bits) = %Zd\n", mpz_sizeinbase(pq, 2), pq);
        gmp_fprintf(stdout, "d (%d bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    // DECRYPT file.

    ss_decrypt_file(input_file, output_file, d, pq);

    // CLOSE public key file and CLEAR variables.

    fclose(pvfile);

    mpz_clears(pq, d, NULL);

    return 0;
}
