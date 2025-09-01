#pragma once

#include <openssl/tls1.h>
#include <openssl/ssl.h>

typedef struct CONNECT_T {
    SSL_CTX *ctx;
    SSL *ssl;
} TLSConnection;

TLSConnection *TLS_init_client();
TLSConnection *TLS_init_server();
void TLS_free_connection(TLSConnection **);
int TLS_init_ssl_for_socket(TLSConnection **tls, int socketfd);