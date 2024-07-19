// Imports
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <gmp.h>

int help();
int encryptFile(char *, char *, char *);
void powMod(mpz_t, mpz_t, mpz_t, mpz_t);
void rsaEncryptFile(FILE *, FILE *, mpz_t, mpz_t);
bool rsaVerify(mpz_t, mpz_t, mpz_t, mpz_t);
int decryptFile(char *, char *, char *);
void rsaDecryptFile(FILE *, FILE *, mpz_t, mpz_t);

int main() {
  return 0;
}

// char* outputFile = "encrypted.txt", char* keyFile = "rsa.pub") {

int mainFunc(bool encrypt, char* inputFile, char* outputFile, char* keyFile) {
  // Declare and define needed variables
  bool changeOut = (strcmp(outputFile, "encrypted.txt") == 0) ? false : true;
  bool changeKey = (strcmp(keyFile, "rsa.pub") == 0) ? false : true;
  if (!changeKey) keyFile = (encrypt) ? "rsa.pub" : "rsa.priv";
  if (!changeOut) outputFile = (encrypt) ? "encrypted.txt" : "decrypted.txt";
  // Main should decide what the user wants to do and call the respective functions.
  return (encrypt) ? encryptFile(inputFile, keyFile, outputFile) : decryptFile(inputFile, keyFile, outputFile);
}

// Encrypt encrypts the file - Not Finished
int encryptFile(char *input, char *key, char *output) {
    // Opens the keyfile | Stores key in keybuffer
    FILE *keyFile = fopen(key, "r");
    if (keyFile == NULL) {
        printf("Keyfile does not exist.\n");
        exit(1);
    }
    // Initilize needed variables
    mpz_t n, e, s, user;
    mpz_inits(n, e, s, user, NULL);
    // Initilization of string being used to get username as 128 allocation is
    // limit of username in unix
    char *test = (char *)calloc(128, sizeof(char));
    // RSA Reads the Public File
    gmp_fscanf(keyFile, "%Zx \n", n);
    gmp_fscanf(keyFile, "%Zx \n", e);
    gmp_fscanf(keyFile, "%Zx \n", s);
    fscanf(keyFile, "%s \n", test);
    fclose(keyFile);
    // Convert username to mpz_t
    mpz_set_str(user, test, 62);
    free(test);
    // Check validity of signature
    if (!rsaVerify(user, s, e, n)) {
        printf("Error, username was not verified.\n");
        mpz_clears(n, e, s, user, NULL);
        exit(1);
    }
    // Open the encrypted file
    FILE *inputFile = fopen(input, "rb");
    if (inputFile == NULL) {
        printf("Input file does not exist.\n");
        mpz_clears(n, e, s, user, NULL);
        exit(1);
    }
    FILE *outputFile = fopen(output, "wb");
    // Encrypts
    rsaEncryptFile(inputFile, outputFile, n, e);
    // Close and clear all used files and mpz_t variables
    mpz_clears(n, e, s, user, NULL);
    fclose(inputFile);
    fclose(outputFile);
    printf("You have successfully encrypted the file \"%s\" into \"%s\"\n", input, output);
    return 0;
}

int decryptFile(char *input, char *key, char *output) {
    // Opens the keyfile | Stores key in keybuffer
    FILE *keyFile = fopen(key, "r");
    if (keyFile == NULL) {
        printf("Keyfile does not exist.\n");
        exit(1);
    }
    // Initilize needed variables
    mpz_t n, d;
    mpz_inits(n, d, NULL);
    // RSA Reads the Private File
    gmp_fscanf(keyFile, "%Zx \n", n);
    gmp_fscanf(keyFile, "%Zx \n", d);
    // Close the Keyfile
    fclose(keyFile);
    // Open the encrypted file
    // piecewise
    FILE *inputFile = fopen(input, "rb");
    if (inputFile == NULL) {
        printf("Input file does not exist.\n");
        mpz_clears(n, d, NULL);
        exit(1);
    }
    FILE *outputFile = fopen(output, "wb");
    // Decrypts
    rsaDecryptFile(inputFile, outputFile, n, d);
    // Close and clear all used files and mpz_t variables
    fclose(inputFile);
    fclose(outputFile);
    mpz_clears(n, d, NULL);
    printf("You have successfully decrypted the file \"%s\" into \"%s\"\n", input, output);
    return 0;
}

int help() {
    printf("%s\n", "Usage: ./main [type] [file] [options]");
    printf("%s\n", "Purpose: This will encrypt or decrypt an input file using the specified public/private key file,");
    printf("%s\n", "  [type] : Specifies if you want to encrypt (0) or decrypt (1) a file.");
    printf("%s\n", "  [file] : The file that you want to encrypt or decrypt.");
    printf("%s\n", "Options:");
    printf("%s\n", "  -o <outfile> : Write output to <outfile>. Default: encrypted.txt or decrypted.txt.");
    printf("%s\n", "  -k <keyfile> : File of the key used for encrypting/decrypting. Default: rsa.pub or rsa.priv.");
    printf("%s\n", "  -h           : Help Menu. Displays program synopsis and usage.");
    printf("\n%s\n", "Example usage: ./main 0 example.txt -o encryptedText.txt -k rsa.pub");
    printf("%s\n", "In the example usage we encrypt (0) a file (example.txt) using the key (rsa.pub). We output it to a file called encryptedText.txt.");
    return 0;
}

// encrypts file using rsa algorithm
void rsaEncryptFile(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {
    // Declare and define needed variables
    uint64_t k;
    size_t j;
    mpz_t m, c;
    // calculate interval for bytes to be read in file
    k = (mpz_sizeinbase(n, 2) - 1) / 8;
    // Initilize needed variables: plaintext and ciphertext, respectively
    mpz_inits(m, c, NULL);
    // declaration of array and prepending 0xFF to fit range of encryption
    uint8_t *arr = (uint8_t *)calloc(k, sizeof(uint8_t));
    arr[0] = 0xFF;
    // check to see if there are remaining bytes to be read in file using j
    while ((j = fread(&arr[1], sizeof(uint8_t), k - 1, infile)) > 0) {
        // convert elements in arr to m
        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, arr);
        // use rsa_encrypt to obtain c: ciphertext
        powMod(c, m, e, n);
        // print c to the outfile: Default: stdout
        gmp_fprintf(outfile, "%Zx ", c);
    }
    // clear all used mpz_t variables and other elements
    free(arr);
    mpz_clears(m, c, NULL);
    return;
}

void rsaDecryptFile(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {
    // Declare and define all needed variables
    uint64_t k;
    size_t j;
    mpz_t c, m;
    // Interval for the reading of the plaintext and ciphertext in the file
    k = (mpz_sizeinbase(n, 2) - 1) / 8;
    // Initilize needed variables in relation to ciphertext and plaintext
    // respectively
    mpz_inits(c, m, NULL);
    uint8_t *arr = (uint8_t *)calloc(k, sizeof(uint8_t));
    while (gmp_fscanf(infile, "%Zx ", c) > 0) {
        powMod(m, c, d, n);
        mpz_export(arr, &j, 1, sizeof(uint8_t), 1, 0, m);
        fwrite(&arr[1], sizeof(uint8_t), j - 1, outfile);
    }
    // clear all used mpz_t variables and elements
    free(arr);
    mpz_clears(c, m, NULL);
    return;
}

void powMod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {
    // Declare and define all needed variables
    mpz_t p, exp, n, v;
    // Initilize needed variables
    mpz_inits(p, exp, n, v, NULL);
    // Set needed variables used for calculating out which is equal to
    // base^exponent (mod modulus)
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
        mpz_mod(p, p, n);           // p^2mod n
        mpz_fdiv_q_ui(exp, exp, 2); // floor division: exp = exp/2
    }
    mpz_set(out, v);
    // clear all used mpz_t variables
    mpz_clears(exp, n, p, v, NULL);
    return;
}

// verifies the validity of username from file using rsa algorithm
bool rsaVerify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {
    // Declare and define needed variables
    mpz_t t;
    // Initilize needed variables
    mpz_init(t);
    // verify signature through T = S^e mod n if it is valid or not
    powMod(t, s, e, n);
    if (mpz_cmp(m, t) == 0) {
        mpz_clear(t);
        return true;
    } else {
        mpz_clear(t);
        return false;
    }
}
