#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <gmp.h>
#include <stdbool.h>
#include <math.h>

gmp_randstate_t state;

#define OPTIONS "b:i:n:d:s:h"

void gcd(mpz_t, mpz_t, mpz_t);
void mod_inverse(mpz_t, mpz_t, mpz_t);
bool is_prime(mpz_t, uint64_t);
void make_prime(mpz_t, uint64_t, uint64_t);
void lcm(mpz_t, mpz_t, mpz_t);
void rsa_make_pub(mpz_t, mpz_t, mpz_t, mpz_t, uint64_t, uint64_t);
void rsa_make_priv(mpz_t, mpz_t, mpz_t, mpz_t);
void pow_mod(mpz_t, mpz_t, mpz_t, mpz_t);
void usage();
 
// main function for switch cases
int main(int argc, char **argv) {
	// Declare and define needed variables
	int opt = 0;
	uint64_t bits = 1024;
	uint64_t iters = 50;
	bool check_seed = false;
	int seed = 0;
	char *pub = "rsa.pub";
	char *priv = "rsa.priv";
   FILE *pbfile = NULL;
	FILE *pvfile = NULL;
	char *user = getenv("USER");	
   mpz_t use, signature, p, q, n, e, d;
	// Initilize needed variables
	mpz_inits(p, q, n, e, d, use, signature, NULL);
	// command line options ofr switch statement
	while ((opt = getopt(argc, argv, OPTIONS)) != -1) {
		switch (opt) {
			case 'b': 
				bits = (uint64_t) strtoul(optarg, NULL, 10); 
				break;
			case 'i': 
				iters = (uint64_t) strtoul(optarg, NULL, 10); 
				break;
			case 'n': 
				pub = optarg; 
				break;
			case 'd': 
				priv = optarg; 
				break;
			case 's':
				seed = (uint64_t) strtoul(optarg, NULL, 10);
				check_seed = true;
				break;
			case 'h': 
				usage(); 
				return 0;
		default:
			usage();
			exit(1);
			break;
		}
	}
	//open private and public key files for writing in terms of operator input
	pbfile = fopen(pub, "w");
	pvfile = fopen(priv, "w");
	// obtains username to verify who is using keys
	uint64_t fi = fileno(pvfile);
	fchmod(fi, S_IRUSR | S_IWUSR);
	// check seed
	if (check_seed == false) {
		// Initilization of seed value
		gmp_randinit_mt(state);
		// calls to gmp_randseed_ui()
		gmp_randseed_ui(state, time(NULL));
		//calls to srandom() using this seed
		srandom(time(NULL));
	} 
	else {
		gmp_randinit_mt(state);
		// calls to gmp_randseed_ui()
		gmp_randseed_ui(state, seed);
		//calls to srandom() using this seed
		srandom(seed);
	}
	if (bits < 50 || bits > 4096) {
		mpz_clears(p, q, n, e, d, use, signature, NULL);
		gmp_randclear(state);
		fclose(pvfile);
    	fclose(pbfile);
		printf("Keygen failed. Bit option should be between 50 and 4096.\n");
		exit(1);
	}
	// generates the public and private keys
	rsa_make_pub(p, q, n, e, bits, iters);
	rsa_make_priv(d, e, p, q);
	// convert username into a mpz_t with base 62
	mpz_set_str(use, user, 62);
	// Signs the signature using RSA
	pow_mod(signature, use, d, n);
	// RSA Write Public Key
	gmp_fprintf(pbfile, "%Zx \n", n);
	gmp_fprintf(pbfile, "%Zx \n", e);
	gmp_fprintf(pbfile, "%Zx \n", signature);
	fprintf(pbfile, "%s \n", user);
	// RSA Write Priv
	gmp_fprintf(pvfile, "%Zx \n", n);
	gmp_fprintf(pvfile, "%Zx \n", d);
	// clear and close all used files and mpz_t variables
   mpz_clears(p, q, n, e, d, use, signature, NULL);
   fclose(pvfile);
   fclose(pbfile);
   gmp_randclear(state);
	printf("Keygen success. Have fun encrypting!\n");
	return 0;
}

int keygen() {
	// Declare and define needed variables
	uint64_t bits = 1024;
	uint64_t iters = 50;
	char *pub = "rsa.pub";
	char *priv = "rsa.priv";
    FILE *pbfile = NULL;
    FILE *pvfile = NULL;
    char *user = getenv("USER");	
    mpz_t use, signature, p, q, n, e, d;
	// Initilize needed variables
	mpz_inits(p, q, n, e, d, use, signature, NULL);
	//open private and public key files for writing in terms of operator input
	pbfile = fopen(pub, "w");
	pvfile = fopen(priv, "w");
	// obtains username to verify who is using keys
	uint64_t fi = fileno(pvfile);
	fchmod(fi, S_IRUSR | S_IWUSR);
	// check seed
	// Initilization of seed value
	gmp_randinit_mt(state);
	// calls to gmp_randseed_ui()
	gmp_randseed_ui(state, time(NULL));
	//calls to srandom() using this seed
	srandom(time(NULL));
	// generates the public and private keys
	rsa_make_pub(p, q, n, e, bits, iters);
	rsa_make_priv(d, e, p, q);
	// convert username into a mpz_t with base 62
	mpz_set_str(use, user, 62);
	// Signs the signature using RSA
	pow_mod(signature, use, d, n);
	// RSA Write Public Key
	gmp_fprintf(pbfile, "%Zx \n", n);
	gmp_fprintf(pbfile, "%Zx \n", e);
	gmp_fprintf(pbfile, "%Zx \n", signature);
	fprintf(pbfile, "%s \n", user);
	// RSA Write Priv
	gmp_fprintf(pvfile, "%Zx \n", n);
	gmp_fprintf(pvfile, "%Zx \n", d);
	// clear and close all used files and mpz_t variables
    mpz_clears(p, q, n, e, d, use, signature, NULL);
    fclose(pvfile);
    fclose(pbfile);
    gmp_randclear(state);
	printf("Keygen success. Have fun encrypting!\n");
	return 0;
}

// Computes the greatest common divisor of storing the value of the computed divisor in d in relation to Euclidean algorithm
void gcd(mpz_t d, mpz_t a, mpz_t b) {
	// Declare and define all needed variables
	mpz_t temp, temp1, temp2;
	mpz_inits(temp, temp1, temp2, NULL);
	// Set the needed temp variables
	mpz_set(temp1, b); mpz_set(temp2, a);
	while (mpz_cmp_ui(temp1, 0) != 0) {
		mpz_set(temp, temp1);
		mpz_mod(temp1, temp2, temp1); //the loop iterates and keeps using modulus until b is equal to 0
		mpz_set(temp2, temp);
	}
	// temporary mpz_t variables set to putput mpz_t variables
	mpz_set(d, temp2);
	// clear all mpz_t variables used
	mpz_clears(temp, temp1, temp2, NULL);
	return;
}

// Computing base raised to the exponent power modulo modulus, and storing the computed result.
void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
  	// Declare and define all needed variables
	mpz_t p, exp, n, v;
	// Initilize needed variables
	mpz_inits(p, exp, n, v, NULL);
	// Set needed variables used for calculating out which is equal to base^exponent (mod modulus)
	mpz_set(p, base);
	mpz_set(exp, exponent);
	mpz_set(n, modulus);
	mpz_set_ui(v, 1);
	// Check to see if exponenet is not zero
	while (mpz_cmp_ui(exp, 0) > 0) {
		if (mpz_odd_p(exp) != 0) {
			mpz_mul(v, v, p);
			mpz_mod(v, v, n);
		}
		mpz_mul(p, p, p);
		mpz_mod(p, p, n); //p^2mod n
		mpz_fdiv_q_ui(exp, exp, 2); //floor division: exp = exp/2
	}
	mpz_set(out, v);
	// clear all used mpz_t variables
	mpz_clears(exp, n, p, v, NULL);
	return;
}

// Using miller-rabin primality test to verify if integer is prime or not 
bool is_prime(mpz_t n, uint64_t iters) {
	// Declare and define needed variables
	mpz_t dividend, s, r, div, result, a, y, n_minus, s_minus, j;
	// Initilize needed variables
	mpz_inits(dividend, s, div, result, r, a, y, n_minus, s_minus, j, NULL);
	// Compute and set needed variables in order to not change original parameters
	mpz_set(dividend, n);
	mpz_sub_ui(dividend, dividend,1);
	mpz_set_ui(div,1);
	mpz_set(result, n);
	mpz_sub_ui(result, result, 3);
	mpz_sub_ui(n_minus, n, 1);
	// find s and r reapeadly dividing n-1 by 2 until r is odd
	while (mpz_odd_p(r) == 0) {
		mpz_add_ui(s, s, 1);
		mpz_div(r, dividend, div);
		mpz_mul_ui(div, div, 2);
	}
	mpz_sub_ui(s, s, 1);
	mpz_sub_ui(s_minus, s, 1);
	// iterate form 1 to specified number by operator: Default: 50
	for (uint64_t i = 1; i < iters; i++) {
		// call u_ramdomm to generate numbers from 0 to n-1 inclusive
		mpz_urandomm(a, state, result);
		// increment a by 2 to set interval from 2 to n-2	
		mpz_add_ui(a, a, 2); 
		// call pow_mod to set y to a^r (mod n)
		pow_mod(y, a, r, n);
		// check if y is not 1 and y is not n-1
		if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_minus) != 0) {
			// set j to 1
			mpz_set_ui(j, 1); //sets j to 1

			// check that j is less than or equal to s-1 and y is not equal to n-1
			while (mpz_cmp(j, s_minus) <= 0 && mpz_cmp(y, n_minus) != 0) {
				mpz_mul(y, y, y);
				mpz_mod(y, y, n); // calculates y^2 mod n
				if (mpz_cmp_ui(y, 1) == 0) {
					mpz_clears(dividend, s, r, div, y, a, s_minus, n_minus, j, result, NULL);
					// return false if y equal to 1
					return false;
				}
				// increment j by 1	
				mpz_add_ui(j, j, 1);
			}
			if (mpz_cmp(y, n_minus) != 0) {
				mpz_clears(dividend, s, r, div, y, a, s_minus, n_minus, j, result, NULL);
				// return false if y is not equal to n-1
				return false;
			}
		}
	}
	// clear all used mpz_t variables
	mpz_clears(dividend, s, r, div, y, a, s_minus, n_minus, j, result, NULL);
	// return true if all prime tests pass
	return true;
}

// generate random prime numbers
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {
	// Declare and define needed variables
	mpz_t min;
	// Initialize needed variables
	mpz_init(min);
	// Generate random prime number between 0 and 2^bits-1 long
	mpz_ui_pow_ui(min, 2, bits);
	mpz_urandomb(p, state, bits);
	// add 2^bits to the result in order for the number is at least bits number of bits long
	mpz_add(p, p, min);
	// check to see if the random number is prime number
	while (is_prime(p, iters) == false) {
		// make random numbers bits long until number is prime
		mpz_urandomb(p, state, bits);
		mpz_add(p, p, min);
	}
	// clear all mpz_t variables
   	mpz_clear(min);
	return;
}

// function to calculate the modular inverse
void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {
	// Declare and define needed variables
	mpz_t r, r_prime, t, t_prime, q, q_product_r, q_product_t, r_sub, t_sub, temp_r, temp_i;
	// Initilize needed variables
	mpz_inits(r, r_prime, t, t_prime, q, q_product_r, q_product_t, r_sub, t_sub, temp_r, temp_i, NULL);
	// Set needed variables and 
	mpz_set(r, n);
	mpz_set(r_prime, a);
	mpz_set_ui(t, 0);
	mpz_set_ui(t_prime, 1);
	// check if r prime is zero
	while (mpz_cmp_ui(r_prime, 0) != 0) {
	// calculate q to be equal to floor (r/r_prime), and switch between r and r_prime
		mpz_fdiv_q(q, r, r_prime);
		mpz_mul(q_product_r, q, r_prime);
		mpz_mul(q_product_t, q, t_prime);
		mpz_sub(r_sub, r, q_product_r);
		mpz_sub(t_sub, t, q_product_t);
		mpz_set(temp_r, r_prime);
		mpz_set(temp_i, t_prime);
		mpz_set(r, temp_r);
		mpz_set(r_prime, r_sub);
		mpz_set(t, temp_i);
		mpz_set(t_prime, t_sub);
	}
	// return no inverse if r is greater than 1 
	if (mpz_cmp_ui(r, 1) > 0) {
		mpz_clears(r, r_prime, t, t_prime, q, q_product_r, q_product_t, r_sub, t_sub, temp_r, temp_i, NULL);
		return;
	}
	// increment t by n if less than zero
	if (mpz_cmp_ui(t, 0) < 0) { 
		mpz_add(t, t, n);
	}
	mpz_set(i, t);
	// clear all used mpz_t variables
	mpz_clears(r, r_prime, t, t_prime, q, q_product_r, q_product_t, r_sub, t_sub, temp_r, temp_i, NULL);
	return;
}

// makes public key
void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {
	// Declare and define needed variables
	mpz_t p_minus, q_minus, least, tester;
	// Initilize needed variables
	mpz_inits(q_minus, p_minus, tester, least, NULL);
	// calculates respective bit sizes for p and q
	uint64_t size = (random() % (3 * nbits / 4 - nbits / 4 + 1)) + nbits / 4;
	uint64_t remain = nbits - size;
	//calculates the respective bit sizes for p and q
	make_prime(p, size, iters);
	make_prime(q, remain, iters);
	mpz_mul(n, p, q);
	//generates p and q and then calculates n by taking the product of p and q
	mpz_sub_ui(q_minus, q, 1);
	mpz_sub_ui(p_minus, p, 1);
	// find lcm p-1 and q-1 to find coprimes of p and q
	lcm(least, p_minus, q_minus);
	// generate public key between 0 and 2^bits-1 long
	do {
	// loop if not satisfy gcd (e, lcm(p-1, q-1)) not zero
		mpz_urandomb(e, state, nbits);
		gcd(tester, e, least);
	} while (mpz_cmp_ui(tester, 1) != 0);
	// clear all used mpz_t variables
   	mpz_clears(q_minus, p_minus, tester, least, NULL);
	return;
}

// makes private key using rsa algorithm
void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {
	// Declare and define all needed variables
	mpz_t lambda, p_minus, q_minus;
	// Initilize all needed variables
	mpz_inits(lambda, p_minus, q_minus, NULL);
	// Generate private key through lcm(p-1, q-1) and finding modular inverse e of public key
	mpz_sub_ui(p_minus, p, 1);
	mpz_sub_ui(q_minus, q, 1);
	lcm(lambda, p_minus, q_minus);
	mod_inverse(d, e, lambda);
	mpz_clear(lambda);
	// clear all used mpz_t variables
	mpz_clears(p_minus, q_minus, NULL);
	return;
}

// lowest common denominator helper function
void lcm(mpz_t d, mpz_t a, mpz_t b) {
	// Declare all needed variables
	mpz_t product, x, temp, temp2;
	// Initilize needed variables
	mpz_inits(x, product, temp, temp2, NULL);
	// Set needed variables
	mpz_set(temp, a);
	mpz_set(temp2, b);
	// use gcd function to simulate abs (a*b)/ gcd (a,b)
	mpz_mul(product, temp, temp2);
	mpz_abs(product, product);
	gcd(x, a, b);
	mpz_div(d, product, x);
	// Clear all used mpz_t variables
	mpz_clears(x, product, temp, temp2, NULL);
	return;
}

void usage() {
	printf ("%s\n", "Usage: ./keygen-dist [options]");
	printf ("%s\n", "  ./keygen-dist generates a public / private key pair, placing the keys into the public and private");
	printf ("%s\n", "  key files as specified below. The keys have a modulus (n) whose length is specified in");
	printf ("%s\n", "  the program options.");
	printf ("%s\n", "    -s <seed>   : Use <seed> as the random number seed. Default: time()");
	printf ("%s\n", "    -b <bits>   : Public modulus n must have at least <bits> bits. Default: 1024");
	printf ("%s\n", "    -i <iters>  : Run <iters> Miller-Rabin iterations for primality testing. Default: 50");
	printf ("%s\n", "    -n <pbfile> : Public key file is <pbfile>. Default: rsa.pub");
	printf ("%s\n", "    -d <pvfile> : Private key file is <pvfile>. Default: rsa.priv");
	printf ("%s\n", "    -h          : Display program synopsis and usage.");
	return;
}
