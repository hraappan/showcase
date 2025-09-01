#include "tls-connection.h"
#include "logging.h"

static void tls_info_callback(const SSL *ssl, int where, int ret) {
    const char *state = "";
    
    if (where & SSL_CB_HANDSHAKE_START) {
        printf("[TLS] Handshake started\n");
    }
    
    if (where & SSL_CB_LOOP) {
        state = SSL_state_string_long(ssl);
        printf("[TLS] LOOP: state=%s\n", state);
    }
    
    if (where & SSL_CB_EXIT) {
        state = SSL_state_string_long(ssl);
        if (ret == 0) {
            printf("[TLS] EXIT: state=%s, handshake failed\n", state);
        } else if (ret < 0) {
            printf("[TLS] EXIT: state=%s, handshake error\n", state);
        } else {
            printf("[TLS] EXIT: state=%s, handshake ok\n", state);
        }
    }
    
    if (where & SSL_CB_ALERT) {
        int alert_type = (where >> 8) & 0xff; // alert type
        int alert_desc = where & 0xff;       // alert description
        printf("[TLS] ALERT: type=%s, desc=%s (%d)\n",
               SSL_alert_type_string_long(alert_type),
               SSL_alert_desc_string_long(alert_desc),
               alert_desc);
    }
    
    if (where & SSL_CB_READ) {
        // optional: data read
    }
    if (where & SSL_CB_WRITE) {
        // optional: data written
    }
}

static void TLS_server_set_options(TLSConnection *tls)
{
    if (tls == NULL || tls->ctx == NULL) return;
    uint64_t opts;

    /**
     * Tolerate hanging up without TLS shutdown.
     */
    opts = SSL_OP_IGNORE_UNEXPECTED_EOF;
    /**
     *  Block CPU exhaustion attacks.
     */
    opts |= SSL_OP_NO_RENEGOTIATION;
    /**
     *  Usage of own cipher rather than client.
     */
    opts |= SSL_OP_CIPHER_SERVER_PREFERENCE;

    SSL_CTX_set_options(tls->ctx, opts);
}

TLSConnection *TLS_init_client()
{
    const SSL_METHOD *method = TLS_client_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (ctx == NULL) {
        ERROR_PRINT("Creation of SSL CTX object failed");
        return NULL;
    }

    SSL_CTX_set_info_callback(ctx, tls_info_callback);

    TLSConnection *tls = malloc(sizeof(TLSConnection));
    if (!tls) {
        ERROR_PRINT("Could not allocate memory for tls connection");
        SSL_CTX_free(ctx);
        return NULL;
    }

    //Avoid older versions because they are deprecated.
    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION)) {
        ERROR_PRINT("Could not set minimum proto version for TLS connection");
        SSL_CTX_free(ctx);
        return NULL;
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
    tls->ctx = ctx;
    tls->ssl = NULL;
    return tls;
}

TLSConnection *TLS_init_server()
{
    const SSL_METHOD *method = TLS_server_method();
    SSL_CTX *ctx = SSL_CTX_new(method);
    if (ctx == NULL) {
        ERROR_PRINT("Creation of SSL CTX object failed");
        return NULL;
    }

    // Salli laaja cipher-lista
    if (!SSL_CTX_set_cipher_list(ctx, "ALL:@SECLEVEL=0")) {
    ERR_print_errors_fp(stderr);
}
    SSL_CTX_set_info_callback(ctx, tls_info_callback);
    TLSConnection *tls = malloc(sizeof(TLSConnection));
    if (!tls) {
        ERROR_PRINT("Could not allocate memory for tls connection");
        SSL_CTX_free(ctx);
        return NULL;
    }

    //Avoid older versions because they are deprecated.
    if (!SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION)) {
        ERROR_PRINT("Could not set minimum proto version for TLS connection");
        SSL_CTX_free(ctx);
        return NULL;
    }

    tls->ctx = ctx;
    tls->ssl = NULL;
    //TLS_server_set_options(tls);
    return tls;
}


void TLS_free_connection(TLSConnection **tls)
{
    if (tls == NULL || *tls == NULL) return;
    if ((*tls)->ctx != NULL)
        SSL_CTX_free((*tls)->ctx);
    if ((*tls)->ssl != NULL)
        SSL_free((*tls)->ssl);
    free((*tls));
}

int TLS_init_ssl_for_socket(TLSConnection **tls, int socketfd)
{
    SSL *ssl;
    ssl = SSL_new((*tls)->ctx);
    if (ssl == NULL) {
        ERROR_PRINT("Could not get ssl structure");
        return -1;
    }

    (*tls)->ssl = ssl;
    if (!SSL_set_fd(ssl, socketfd)) {
        ERROR_PRINT("Could not set file descriptor for ssl");
        return -1;
    }

    return 1;
}
