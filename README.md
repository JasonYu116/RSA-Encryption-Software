# RSA Encryption Program
This program encrypts user files and decrypts ciphertext using a RSA algorithm given a user input through a user interface.
The program features a graphical user interface for the user to interact and provide input through as well as contains .exe files for standalone
utilization. Do note that the program is only usable on Linux Ubuntu or similar systems only.

# Files
- keygen.c contains the code for RSA private and public key generation.
- main.c contains the code for the main functions for RSA and RSA algorithm.
- main.py contains the executable file for the RSA encryption Program.
- keygen.so contains the code to allow the generation of keygen.exe file to run as a standalone file.
- main.so contains the code to allow the generation of main.exe file to run as a standalone file.
- Makefile contains the code to generate the executable file.
- example.txt is a test file and is not necessary for the program to work.
- README contains the program usage and documentation for the project.
 
## Building
Run the following to build the RSA Encryption Program
```
python3 main.py
```
Run the following to build the standalone RSA Encryption Program
```
./main.exe
```

## Running 
See User interface for more specific instructions.  

Steps to encrypt and decrypt file are as follows:  
1. Generate public and private keys through the "Generate a Key" button.  
2. Add the file you want to encrypt through "Add File".  
3. Add .pub and .priv files to add a public key or private key. (There's a button for that!)  
4. Click on "Encrypt" and choose where to encrypt the file to and what to name it.  
5. Toggle to decrypt and choose what file to decrypt and what name and extension that you want your decrypted file to be.  
Further details can be found by hovering over the buttons.

## Sources Consulted
The following sources were consulted:  
Man files in bash through terminal  
gmp library: https://gmplib.org/  
Youtube and Stackoverflow

# Credits
Kevin Yosifov - Programmer
Jason Yu      - Programmer