#pragma once
#include <openssl/evp.h>

typedef struct MD {
    EVP_MD_CTX *ctx;
    EVP_MD *sha256;
} MessageDigest;

MessageDigest *MD_digest_init(void *);
int MD_update_message(MessageDigest *md, void *msg, size_t msg_size);
int MD_calculate_digest(MessageDigest *md, unsigned char *out, unsigned int *len);