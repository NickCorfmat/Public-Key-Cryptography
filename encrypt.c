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

    char username[LOGIN_NAME_MAX];
    char *pub_file = "ss.pub";
    char *in_name = "default_input";
    char *out_name = "default_output";

    mpz_t n;

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
        case 'n': // SPECIFY public key file.
            pub_file = optarg;

            break;
        case 'v': // ENABLE verbose output.
            verbose_output = true;

            break;
        case 'h': // DISPLAY program usage.
            printf("SYNOPSIS\n");
            printf("   Encrypts data using SS encryption.\n");
            printf("   Encrypted data is decrypted by the decrypt program.\n\n");
            printf("USAGE\n");
            printf("   ./encrypt [OPTIONS]\n\n");
            printf("OPTIONS\n");
            printf("   -h              Display program help and usage.\n");
            printf("   -v              Display verbose program output.\n");
            printf("   -i infile       Input file of data to encrypt (default: stdin).\n");
            printf("   -o outfile      Output file for encrypted data (default: stdout).\n");
            printf("   -n pbfile       Public key file (default: ss.pub).\n");

            break;
        }
    }

    // OPEN the public key file.

    FILE *pbfile = fopen(pub_file, "r");

    if (pbfile == NULL) {
        fprintf(stderr, "Error: Encrypt could not access public key file.\n");
        return 1;
    }

    // CREATE and OPEN input and output files.

    FILE *input_file;
    FILE *output_file;

    if (toggle_i) {
        input_file = fopen(in_name, "r");
        if (input_file == NULL) {
            fprintf(stderr, "Error: Encrypt could not access input file.\n");
            return 1;
        }
    } else {
        input_file = stdin;
    }

    if (toggle_o) {
        output_file = fopen(out_name, "w");
        if (output_file == NULL) {
            fprintf(stderr, "Error: Encrypt could not access output file.\n");
            return 1;
        }
    } else {
        output_file = stdout;
    }

    // INITIALIZE multiple-precision variables.

    mpz_init(n);

    // READ the public key from the public key file.

    ss_read_pub(n, username, pbfile);

    // DO if verbose output is enabled.

    if (verbose_output) {
        gmp_fprintf(stdout, "user = %s\n", username);
        gmp_fprintf(stdout, "n (%d bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
    }

    // ENCRYPT file.
    ss_encrypt_file(input_file, output_file, n);

    // CLOSE public key file and CLEAR variables.

    fclose(pbfile);

    mpz_clear(n);

    return 0;
}
