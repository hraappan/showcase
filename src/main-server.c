#include "logging.h"
#include "tls-connection.h"
#include "session.h"
#include "certificate.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

static const char *ip = "127.0.0.1";
static const uint16_t port = 6666;

static pthread_t cthread;

void *handle_connection(void *client)
{

}

int main(__attribute__((__unused__)) int argc, __attribute__((__unused__)) char *argv[])
{
    struct sockaddr_in client_addr, server_addr;
    socklen_t client_len = sizeof(client_addr);
    socklen_t server_len = sizeof(server_addr);

    uint8_t read_buffer[64];

    INFO_PRINT("Going to start TCP server.");
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        ERROR_PRINT("Could not get file descriptor for socket, errno %i.", errno);
        return -1;
    }

    memset((void *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port   = htons(port);
    if (inet_aton(ip, &server_addr.sin_addr) == 0) {
        ERROR_PRINT("Address is not valid %s", ip);
        close(server_sock);
        return -1;
    }

    if (bind(server_sock, (struct sockaddr *) &server_addr, server_len) < 0) {
        ERROR_PRINT("Cannot bind %i", errno);
        close(server_sock);
        return -1;
    }

    if (listen(server_sock, 2) < 0) {
        ERROR_PRINT("Error %i occurred when calling listen to socket", errno);
        close(server_sock);
        return -1;
    }

    TLSConnection *tls = TLS_init_server();
    if (tls == NULL) {
        ERROR_PRINT("Cannot create TLS connection");
        close(server_sock);
        return -1;
    }

    if (!TLS_server_set_client_verification(&tls, true)) {
        ERROR_PRINT("Could not set client verification.");
        close(server_sock);
        TLS_free_connection(&tls);
        return -1;
    }

    if (CA_certificate_file(&tls) != 1) {
        ERROR_PRINT("Cannot set server certificate");
        close(server_sock);
        TLS_free_connection(&tls);
        return -1;
    }
    if (CA_certificate_priv_file(&tls) != 1) {
        ERROR_PRINT("Cannot set server private key");
        close(server_sock);
        TLS_free_connection(&tls);
        return -1;
    }

    while (1) {
        INFO_PRINT("Server waiting for connection");
        int client_fd = accept(server_sock, (struct sockaddr *) &client_addr, &client_len);
        if (client_fd < 0) {
            ERROR_PRINT("Error %i occurred while accept connection.", errno);
            close(server_sock);
            TLS_free_connection(&tls);
            return -1;
        } else {
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
            int client_port = ntohs(client_addr.sin_port);

            getsockname(client_fd, (struct sockaddr *)&server_addr, &server_len);

            char server_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, sizeof(server_ip));
            int server_port = ntohs(server_addr.sin_port);

            DEBUG_PRINT("Connection accepted: client %s:%d -> server %s:%d",
                    client_ip, client_port, server_ip, server_port);
        }

        if (!TLS_init_ssl_for_socket(&tls, client_fd)) {
            ERROR_PRINT("Could not initialize tls for socket.");
            close(server_sock);
            TLS_free_connection(&tls);
            return -1;
        }

        int ret = SSL_accept(tls->ssl);
        if (ret != 1) {
            int err = SSL_get_error(tls->ssl, ret);
            fprintf(stderr, "SSL handshake failed: %d\n", err);
            ERROR_PRINT("%s", stderr);
            ERROR_PRINT("TLS/SSL handshake was not successfull, err %i", err);
            SSL_free(tls->ssl);
            continue;
        }

        SSL_read(tls->ssl, read_buffer, sizeof(read_buffer));
        INFO_PRINT("Received buffer %s", read_buffer);
    }

    TLS_free_connection(&tls);
    close(server_sock);
    INFO_PRINT("Socket is now closed.");
    return 0;
}