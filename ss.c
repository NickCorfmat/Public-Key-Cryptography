#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <gmp.h>

#include "ss.h"
#include "randstate.h"
#include "numtheory.h"

void ss_make_pub(mpz_t p, mpz_t q, mpz_t n, uint64_t nbits, uint64_t iters) {
    mpz_t p_squared, p_minus_1, q_minus_1, p_mod_q, q_mod_p;

    // INITIALIZE mpz objects.
    mpz_inits(p_squared, p_minus_1, q_minus_1, p_mod_q, q_mod_p, NULL);

    // LOOP while log(n) < 2 OR p is not divisible by (q - 1) OR q is not divisible by (p - 1).
    do {
        // COMPUTE p bits and q bits.
        uint64_t p_bits = (random() % (((2 * nbits) / 5) - (nbits / 5))) + (nbits / 5);
        uint64_t q_bits = nbits - (2 * p_bits);

        // MAKE primes p and q.
        make_prime(p, p_bits, iters);
        make_prime(q, q_bits, iters);

        // COMPUTE p % (q - 1) and q % (p - 1).
        mpz_sub_ui(p_minus_1, p, 1);
        mpz_sub_ui(q_minus_1, q, 1);

        mpz_mod(p_mod_q, p, q_minus_1);
        mpz_mod(q_mod_p, q, p_minus_1);

        // COMPUTE key n.
        mpz_mul(p_squared, p, p);
        mpz_mul(n, p_squared, q);

    } while (
        mpz_sizeinbase(n, 2) < nbits || mpz_cmp_ui(p_mod_q, 0) == 0 || mpz_cmp_ui(p_mod_q, 0) == 0);

    // DEALLOCATE mpz objects.
    mpz_clears(p_squared, p_minus_1, q_minus_1, p_mod_q, q_mod_p, NULL);
}

void ss_write_pub(const mpz_t n, const char username[], FILE *pbfile) {
    gmp_fprintf(pbfile, "%Zx\n%s\n", n, username);
}

void ss_read_pub(mpz_t n, char username[], FILE *pbfile) {
    gmp_fscanf(pbfile, "%Zx\n%s\n", n, username);
}

void ss_make_priv(mpz_t d, mpz_t pq, const mpz_t p, const mpz_t q) {
    mpz_t n, p_minus_1, q_minus_1, lambda, numerator, denominator;

    // INITIALIZE mpz objects.
    mpz_inits(n, p_minus_1, q_minus_1, lambda, numerator, denominator, NULL);

    // COMPUTE p_minus_1 and q_minus_1.
    mpz_sub_ui(p_minus_1, p, 1);
    mpz_sub_ui(q_minus_1, q, 1);

    // COMPUTE lcm(p - 1, q - 1).
    mpz_mul(numerator, p_minus_1, q_minus_1);
    gcd(denominator, p_minus_1, q_minus_1);
    mpz_fdiv_q(lambda, numerator, denominator);

    // Compute key n.
    mpz_mul(pq, p, q);
    mpz_mul(n, pq, p);

    // COMPUTE SS private key d.
    mod_inverse(d, n, lambda);

    // DEALLOCATE mpz objects.
    mpz_clears(n, p_minus_1, q_minus_1, lambda, numerator, denominator, NULL);
}

void ss_write_priv(const mpz_t pq, const mpz_t d, FILE *pvfile) {
    gmp_fprintf(pvfile, "%Zx\n%Zx\n", pq, d);
}

void ss_read_priv(mpz_t pq, mpz_t d, FILE *pvfile) {
    gmp_fscanf(pvfile, "%Zx\n%Zx\n", pq, d);
}

void ss_encrypt(mpz_t c, const mpz_t m, const mpz_t n) {
    pow_mod(c, m, n, n);
}

void ss_encrypt_file(FILE *infile, FILE *outfile, const mpz_t n) {
    mpz_t m, sqrt_n, encrypted_num;

    // INITIALIZE mpz objects.
    mpz_inits(m, sqrt_n, encrypted_num, NULL);

    size_t j;

    // COMPUTE the square root of n.
    mpz_sqrt(sqrt_n, n);

    // COMPUTE block size k.
    uint64_t k = (mpz_sizeinbase(sqrt_n, 2) - 1) / 8;

    // Dynamically ALLOCATE an array that holds k blocks.
    uint8_t *block = (uint8_t *) malloc(k * sizeof(uint8_t));

    // SET the zeroth byte of the block to 0xFF.
    block[0] = 0xFF;

    // LOOP while there are still unprocessed bytes in 'infile'.
    while (feof(infile) == 0) {
        j = fread(block + 1, sizeof(uint8_t), k - 1, infile);
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);
        ss_encrypt(encrypted_num, m, n);
        gmp_fprintf(outfile, "%Zx\n", encrypted_num);
    }

    // DEALLOCATE both, variables and block.
    free(block);
    mpz_clears(m, sqrt_n, encrypted_num, NULL);
}

void ss_decrypt(mpz_t m, const mpz_t c, const mpz_t d, const mpz_t pq) {
    pow_mod(m, c, d, pq);
}

void ss_decrypt_file(FILE *infile, FILE *outfile, const mpz_t d, const mpz_t pq) {
    mpz_t c, m;

    /// INITIALIZE mpz objects.
    mpz_inits(c, m, NULL);

    size_t j;

    // COMPUTE block size k..
    uint64_t k = (mpz_sizeinbase(pq, 2) - 1) / 8;

    // Dynamically ALLOCATE an array that holds k blocks.
    uint8_t *block = (uint8_t *) malloc(k * sizeof(uint8_t));

    // SET the zeroth byte of the block to 0xFF.
    block[0] = 0xFF;

    // LOOP while there are still unprocessed bytes in 'infile'.
    while (feof(infile) == 0) {
        gmp_fscanf(infile, "%Zx\n", c);
        ss_decrypt(m, c, d, pq);
        mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(block + 1, sizeof(uint8_t), j - 1, outfile);
    }

    // DEALLOCATE both, variables and block.
    free(block);
    mpz_clears(c, m, NULL);
}
