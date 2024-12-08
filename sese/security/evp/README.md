# Encryption and Decryption Functions

Hash digest algorithms can use a unified interface based on Context implementations such as MD5Context, SHA256Context, and SM3Context.

For encryption and decryption related algorithms, such as AES, RSA, and SM4, please directly use the EVP module implemented within OpenSSL.
