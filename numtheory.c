#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <gmp.h>
#include <stdint.h>

#include "randstate.h"
#include "numtheory.h"

void pow_mod(mpz_t out, const mpz_t base, const mpz_t exponent, const mpz_t modulus) {
    mpz_t d, p, v, vp, pp;

    // INITIALIZE variables.
    mpz_inits(d, p, v, vp, pp, NULL);

    // ASSIGN values to variables. SAVING 'base' and 'exponent' to new variables to avoid overwriting them.
    mpz_set_ui(v, 1);
    mpz_set(p, base);
    mpz_set(d, exponent);

    // LOOP while exponent is greater than zero.
    while (mpz_cmp_ui(d, 0) > 0) {
        if (mpz_odd_p(d) == 1) {
            mpz_mul(vp, v, p);
            mpz_mod(v, vp, modulus);
        }

        mpz_mul(pp, p, p);
        mpz_mod(p, pp, modulus);
        mpz_fdiv_q_ui(d, d, 2);
    }

    // SET 'out' to the value calculated.
    mpz_set(out, v);

    // DEALLOCATE memory used by mpz objects.
    mpz_clears(d, p, v, vp, pp, NULL);
}

bool is_prime(const mpz_t n, uint64_t iters) {

    // DEFINE base cases for when number is less than six.
    if (mpz_cmp_ui(n, 1) <= 0) {
        return false;
    }

    if (mpz_cmp_ui(n, 2) == 0 || mpz_cmp_ui(n, 3) == 0 || mpz_cmp_ui(n, 5) == 0) {
        return true;
    }

    if (mpz_even_p(n)) {
        return false;
    }

    mpz_t a, j, r, s, y, n_minus_1, n_minus_3, s_minus_1, two;

    // INITIALIZE mpz objects.
    mpz_inits(a, j, r, s, y, n_minus_1, n_minus_3, s_minus_1, two, NULL);

    // COMPUTE n_minus_1 and m_minus_3.
    mpz_sub_ui(n_minus_1, n, 1);
    mpz_sub_ui(n_minus_3, n, 3);

    // ASSIGN values to s, r, and 'two' variables.
    mpz_set_ui(s, 0);
    mpz_set(r, n_minus_1);
    mpz_set_ui(two, 2);

    // Determine values for s and r SUCH THAT r is ODD.
    while (mpz_even_p(r)) {
        mpz_add_ui(s, s, 1);
        mpz_div_ui(r, r, 2);
    }

    // COMPUTE s_minus_1.
    mpz_sub_ui(s_minus_1, s, 1);

    // LOOP through iters.
    for (uint64_t i = 1; i < iters; i += 1) {

        // Choose a RANDOM element in {2, 3, ..., n - 2}.
        mpz_urandomm(a, state, n_minus_3);
        mpz_add_ui(a, a, 2);

        pow_mod(y, a, r, n);

        // CHECK if y is not equal to one and y is not equal to n_minus_1.
        if ((mpz_cmp_ui(y, 1) != 0) && (mpz_cmp(y, n_minus_1) != 0)) {
            mpz_set_ui(j, 1);

            // LOOP while j <= s_minus_1 and y is not equal to n_minus_1.
            while ((mpz_cmp(j, s_minus_1) <= 0) && (mpz_cmp(y, n_minus_1) != 0)) {
                pow_mod(y, y, two, n);

                // CHECK if y is equal to one.
                if (mpz_cmp_ui(y, 1) == 0) {
                    mpz_clears(a, j, r, s, y, n_minus_1, n_minus_3, s_minus_1, two, NULL);
                    return false;
                }

                mpz_add_ui(j, j, 1);
            }

            // CHECK if y is not equal to n_minus_1.
            if (mpz_cmp(y, n_minus_1) != 0) {

                // DEALLOCATE mpz objects.
                mpz_clears(a, j, r, s, y, n_minus_1, n_minus_3, s_minus_1, two, NULL);
                return false;
            }
        }
    }

    // DEALLOCATE mpz objects.
    mpz_clears(a, j, r, s, y, n_minus_1, n_minus_3, s_minus_1, two, NULL);

    return true;
}

void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    // ASSIGN initial value to p.
    mpz_set_ui(p, 0);

    // LOOP while p is not prime.
    while (!is_prime(p, iters)) {

        // CREATE a random number with 'bits' bits.
        mpz_urandomb(p, state, bits);

        mpz_setbit(p, bits - 1);
    }
}

void gcd(mpz_t d, const mpz_t a, const mpz_t b) {
    mpz_t t, a_temp, b_temp;

    // INITIALIZE mpz objects.
    mpz_inits(t, a_temp, b_temp, NULL);

    // ASSIGN values to variables.
    mpz_set(a_temp, a);
    mpz_set(b_temp, b);

    // LOOP while b is not zero.
    while (mpz_cmp_ui(b_temp, 0) != 0) {
        mpz_set(t, b_temp);
        mpz_mod(b_temp, a_temp, b_temp);
        mpz_set(a_temp, t);
    }

    // ASSIGN the calculated value to d.
    mpz_set(d, a_temp);

    // DEALLOCATE memory used by variables.
    mpz_clears(t, a_temp, b_temp, NULL);
}

void mod_inverse(mpz_t i, const mpz_t a, const mpz_t n) {
    mpz_t r, t, q, r_prime, t_prime, r_temp, t_temp, qr_prime, qt_prime;

    // INITIALIZE mpz objects.
    mpz_inits(r, t, q, r_prime, t_prime, r_temp, t_temp, qr_prime, qt_prime, NULL);

    // ASSIGN values to r and r'.
    mpz_set(r, n);
    mpz_set(r_prime, a);

    // ASSIGN values to t and t'.
    mpz_set_ui(t, 0);
    mpz_set_ui(t_prime, 1);

    // LOOP while r' is not zero.
    while ((mpz_cmp_ui(r_prime, 0) != 0)) {

        // UPDATE r and t temporary variables.
        mpz_set(r_temp, r);
        mpz_set(t_temp, t);

        // UPDATE q.
        mpz_fdiv_q(q, r, r_prime);

        // UPDATE r.
        mpz_set(r, r_prime);

        // UPDATE r'.
        mpz_mul(qr_prime, q, r_prime);
        mpz_sub(r_prime, r_temp, qr_prime);

        // UPDATE t.
        mpz_set(t, t_prime);

        // UPDATE t'.
        mpz_mul(qt_prime, q, t_prime);
        mpz_sub(t_prime, t_temp, qt_prime);
    }

    if (mpz_cmp_ui(r, 1) > 0) {
        mpz_set_ui(t, 0);
    }

    if (mpz_cmp_ui(t, 0) < 0) {
        mpz_add(t, t, n);
    }

    // ASSIGN the calculated value to i.
    mpz_set(i, t);

    // DEALLOCATE memory used by variables.
    mpz_clears(r, q, t, r_prime, r_temp, t_prime, t_temp, qr_prime, qt_prime, NULL);
}
