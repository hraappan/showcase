#include "session.h"
#include "logging.h"
#include "whitelist.h"

/**
 * Session handler for TLS handshake
 * 
 * Allow only whitelisted clients
 * 
 */
static int session_handler(int preverify_ok, X509_STORE_CTX *ctx)
{
    int depth = X509_STORE_CTX_get_error_depth(ctx);
    X509 *cert = X509_STORE_CTX_get_current_cert(ctx);

    if (!preverify_ok) {
        int err = X509_STORE_CTX_get_error(ctx);
        ERROR_PRINT("Certificate verification failed at depth %d: %s\n",
                    depth, X509_verify_cert_error_string(err));
        return 0;
    }

    // Check client cert
    if (depth == 0 && cert) {
        char subject[256];
        X509_NAME_oneline(X509_get_subject_name(cert), subject, sizeof(subject));
        INFO_PRINT("Client cert subject: %s\n", subject);

        // CN whitelist
        if (strstr(subject, ALLOWED_CN) == NULL) {
            INFO_PRINT("Client CN not allowed\n");
            return 0;
        }

        EVP_PKEY *pubkey = X509_get_pubkey(cert);
        if (!pubkey) return 0;

        int allowed = 0;
        int whitelist_s = sizeof(ALLOWED_PUBKEYS) / sizeof(ALLOWED_PUBKEYS[0]);
        for (int i = 0; i<whitelist_s; i++) {
            BIO *bio = BIO_new_mem_buf(ALLOWED_PUBKEYS[i], -1);
            EVP_PKEY *allowed_pub = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
            BIO_free(bio);

            if (!allowed_pub) continue;

            if (EVP_PKEY_eq(pubkey, allowed_pub) == 1) {
                allowed = 1;
                EVP_PKEY_free(allowed_pub);
                break;
            }

            EVP_PKEY_free(allowed_pub);
        }

        EVP_PKEY_free(pubkey);

        if (!allowed) {
            ERROR_PRINT("Client public key not allowed\n");
            return 0;
        }

        INFO_PRINT("Client public key verified");
    }

    return 1;
}

void TLS_set_session_cache_mode(TLSConnection **tls, void *cache_id, unsigned int id_size)
{
    if (!tls || !*tls) return;
    SSL_CTX_set_session_id_context((*tls)->ctx, cache_id, id_size);
    SSL_CTX_set_session_cache_mode((*tls)->ctx, SSL_SESS_CACHE_SERVER);
}

void TLS_set_session_cache_size(TLSConnection **tls, long int cache_size)
{
    if (!tls || !*tls) return;
    SSL_CTX_sess_set_cache_size((*tls)->ctx, cache_size);
}

void TLS_set_session_timeout(TLSConnection **tls, int timeout)
{
    if (!tls || !*tls) return;
    SSL_CTX_set_timeout((*tls)->ctx, timeout);
}

int TLS_server_set_client_verification(TLSConnection **tls, const bool verification)
{
    if (tls == NULL || *tls == NULL) return -1;
    int ret = 0;
    if (verification) {
        INFO_PRINT("Going to set client verification.");
        const char *CA_FILE = getenv("CA_CERT_FILE");
        
        ret = SSL_CTX_load_verify_locations((*tls)->ctx, CA_FILE, NULL);
        if (!ret) {
            ERROR_PRINT("Failed to load CA certificate file: %s, make sure to define CA_CERT_FILE env to proper file location\n", CA_FILE);
            return ret;
        }

        INFO_PRINT("Going to set TLS hand shake.");

        //Client certificate request is sent and TLS handshake fails immediately without.
        SSL_CTX_set_verify((*tls)->ctx,
                           SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                           session_handler);
    } else {
        //Client certificate is not required.
        INFO_PRINT("Certificate is not required");
        SSL_CTX_set_verify((*tls)->ctx, SSL_VERIFY_NONE, NULL);
        return 1;
    }
    
    return ret;
}
