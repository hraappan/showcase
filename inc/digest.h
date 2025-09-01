#pragma once
#include <openssl/evp.h>

#define MD_DIGEST_SIZE(X) EVP_MD_get_size(X)

typedef struct MD_T {
    EVP_MD_CTX *ctx;
    EVP_MD *sha256;
    int md_size;
} MessageDigest;

MessageDigest *MD_digest_init();
int MD_update_message(MessageDigest *md, void *msg, size_t msg_size);
int MD_calculate_digest(MessageDigest *md, unsigned char *out, unsigned int *len);
void MD_digest_free(MessageDigest **md);