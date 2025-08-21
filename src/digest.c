/*
 * digest.c - Message Digest wrapper for OpenSSL EVP API
 *
 * This file provides a simplified interface ("wrapper") around OpenSSL's EVP
 * API for computing SHA-256 digests. The goal is to abstract away the
 * underlying OpenSSL calls, making it easier to initialize, update and
 * finalize digests in a clean and reusable way.
 *
 * Typical usage:
 *
 *   MessageDigest *md = MD_digest_init(NULL);
 *   if (md == NULL) { handle error }
 *
 *   MD_update_message(md, buffer, buffer_len);
 *
 *   unsigned char out[EVP_MAX_MD_SIZE];
 *   unsigned int out_len = 0;
 *   MD_calculate_digest(md, out, &out_len);
 *
 *   MD_digest_free(md);
 *
 * Abstraction benefits:
 * - Keeps OpenSSL-specific calls isolated in one place.
 * - Allows possible future change of the digest algorithm
 *   or even underlying library without breaking the rest of the code.
 * - Provides consistent error handling via logging.
 *
 * License: MIT
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "digest.h"
#include "logging.h"

static const char *md_alg = "SHA256";

/**
 * Internal helper to allocate and bind SHA-256 algorithm to a digest context.
 */
static MessageDigest *MD_set_digest_alg(__attribute__((__unused__)) void *params)
{
    EVP_MD_CTX *ctx = NULL;
    EVP_MD *sha256 = NULL;

    MessageDigest *md = malloc(sizeof(MessageDigest));
    if (md == NULL) return NULL;

    ctx = EVP_MD_CTX_new();
    if (ctx == NULL) {
        ERROR_PRINT("Error occurred while trying to get context for message digest");
        free(md);
        return NULL;
    }

    sha256 = EVP_MD_fetch(NULL, md_alg, NULL);
    if (sha256 == NULL) {
        ERROR_PRINT("Error while trying to get digest algorithm");
        EVP_MD_CTX_free(ctx);
        free(md);
        return NULL;
    }

    if (!EVP_DigestInit_ex(ctx, sha256, NULL)) {
        ERROR_PRINT("Error while trying to bind algorithm to context");
        EVP_MD_CTX_free(ctx);
        EVP_MD_free(sha256);   
        free(md);
        return NULL;
    }

    md->ctx = ctx;
    md->sha256 = sha256;
    return md;
}

/**
 * Update digest state with more message data.
 */
inline int MD_update_message(MessageDigest *md, void *msg, size_t msg_size)
{
    if (!EVP_DigestUpdate(md->ctx, msg, msg_size)) {
        ERROR_PRINT("Could not update digest");
        return -1;
    }
    return 0;
}

/**
 * Finalize digest calculation and write output.
 */
inline int MD_calculate_digest(MessageDigest *md, unsigned char *out, unsigned int *len)
{
    if (!EVP_DigestFinal_ex(md->ctx, out, len)) {
        ERROR_PRINT("Could not calculate digest");
        return -1;
    }
    return 0;
}

/**
 * Public initializer for message digest.
 */
MessageDigest *MD_digest_init(void *params)
{
    return MD_set_digest_alg(params);
}

/**
 * Free digest context and algorithm resources.
 */
void MD_digest_free(MessageDigest *md)
{
    if (md == NULL) return;
    EVP_MD_CTX_free(md->ctx);
    EVP_MD_free(md->sha256);
    free(md);
}
