Nicolas Corfmat
ncorfmat
1890805
# Assignment 5: Public Key Cryptography
```
DESCRIPTION
``` 
This program uses public-private key cryptography to encrypt a user specified message using the generated public key, and then decrypt it using the corresponding private key.
```
BUILD
``` 
To build, run 'make' or 'make all' on the terminal command line within the assignment 5 directory. This creates the 'keygen', 'encrypt', 'decrypt', 'ss', 'numtheory', and 'randstate' executable files which can then be run.
```
CLEAN
```
Running 'make clean' will remove all the executable (.o) files from the assignment 5 directory as well as other compiler-generated files.
```
RUNNING
```
To run, first create the public-private key pair by typing `./keygen` followed by these required arguments: 
+ `-b` followed by the minimum bits needed for the public modulus n (default: 256)
+ `-i` followed by the number of iterations for testing primes (default: 50)
+ `-n` followed by the public key file (default: ss.pub)
+ `-d` followed by the private key file (default: ss.priv)
+ `-s` followed by the seed (default: seconds since the UNIX epoch)
+ `-v` enables verbose output
+ `-h` displays program usage

To encrypt, run `./encrypt` followed by any of these arguments:
+ `-i` followed by the input file (default: stdin)
+ `-o` followed by the output file (default: stdout)
+ `-n` followed by the public key file (default: ss.pub)
+ `-v` enables verbose output
+ `-h` displays program usage

To decrypt, run `./decrypt` followed by any of these arguments:
+ `-i` followed by the user-specified input file (default: stdin)
+ `-o` followed by the user-specified output file (default: stdout)
+ `-n` followed by the private key file (default: ss.priv)
+ `-v` enables verbose output
+ `-h` displaying program usage

```
PIPING
```
To pipe these commands together, separately run `./keygen` first, followed by any of its listed arguments. Then run `./encrypt | ./decrypt` with any valid arguments.
