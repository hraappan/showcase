#include "certificate.h"
#include "logging.h"

int CA_certificate_file(TLSConnection **tls)
{
    const char *cert_file = getenv("TLS_CERT_PATH");
    if (cert_file != NULL) {
        return SSL_CTX_use_certificate_file((*tls)->ctx, cert_file, SSL_FILETYPE_PEM);
    } else ERROR_PRINT("Please set TLS_CERT_PATH");

    return -1;
}

int CA_certificate_priv_file(TLSConnection **tls)
{
    const char *priv_file = getenv("PRIV_KEY_PATH");
    if (priv_file != NULL) {
        return SSL_CTX_use_PrivateKey_file((*tls)->ctx, priv_file, SSL_FILETYPE_PEM);
    } else ERROR_PRINT("Please set PRIV_KEY_PATH");

    return -1;
}