#pragma once

#include <stdbool.h>
#include "tls-connection.h"

/*
 * Servers that want to enable session resumption must specify a cache id
 * byte array, that identifies the server application, and reduces the
 * chance of inappropriate cache sharing.
 */
void TLS_set_session_cache_mode(TLSConnection **tls, void *cache_id, unsigned int id_size);

/*
 * How many client TLS sessions to cache.  The default is
 * SSL_SESSION_CACHE_MAX_SIZE_DEFAULT (20k in recent OpenSSL versions),
 * which may be too small or too large.
 */
void TLS_set_session_cache_size(TLSConnection **tls, long int cache_size);

/*
 * Sessions older than this are considered a cache miss even if still in
 * the cache.  The default is two hours.  Busy servers whose clients make
 * many connections in a short burst may want a shorter timeout, on lightly
 * loaded servers with sporadic connections from any given client, a longer
 * time may be appropriate.
 */
void TLS_set_session_timeout(TLSConnection **tls, int timeout);

/*
 * 
 */
int TLS_server_set_client_verification(TLSConnection **tls, const bool verification);