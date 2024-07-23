# 加解密功能

散列摘要算法可以使用例如 MD5Context、SHA256Context、SM3Context 等基于 Context 实现的统一接口。

加密解密相关的算法，例如 AES、RSA、SM4 等，请直接使用 OpenSSL 内实现的 EVP 模块。