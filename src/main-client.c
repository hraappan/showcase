
#include "logging.h"
#include "tls-connection.h"
#include "session.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static const char* ip = "127.0.0.1";
static const uint16_t port = 6666;

int main(int argc, char *argv[])
{
    struct sockaddr_in sockaddr;
    socklen_t server_len = sizeof(sockaddr);
    uint8_t *write_buffer = { "test message" };

    INFO_PRINT("Going to start tcp-client.");
    int tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_sock < 0) {
        ERROR_PRINT("Failed to create socket.");
        return -1;
    }

    memset(&sockaddr, 0, sizeof(sockaddr));
    sockaddr.sin_port = htons(port);
    sockaddr.sin_family = AF_INET;

    if (inet_aton(ip, &sockaddr.sin_addr) == 0) {
        ERROR_PRINT("Address is not valid %s", ip);
        close(tcp_sock);
        return -1;
    }
    
    TLSConnection *tls = TLS_init_client();
    if (tls == NULL) {
        ERROR_PRINT("Cannot initialize client");
        close(tcp_sock);
        return -1;
    }

    if (CA_certificate_file(&tls) != 1) {
        ERROR_PRINT("Cannot set client certificate");
        close(tcp_sock);
        TLS_free_connection(&tls);
        return -1;
    }
    if (CA_certificate_priv_file(&tls) != 1) {
        ERROR_PRINT("Cannot set client private key");
        close(tcp_sock);
        TLS_free_connection(&tls);
        return -1;
    }

    if (connect(tcp_sock, (struct sockaddr *) &sockaddr, server_len) < 0) {
        ERROR_PRINT("Cannot connect to server ip %s, errno %i", ip, errno);
        TLS_free_connection(&tls);
        close(tcp_sock);
        return -1;
    }

    if (!TLS_init_ssl_for_socket(&tls, tcp_sock)) {
        ERROR_PRINT("Could not initialize tls for socket.");
        close(tcp_sock);
        TLS_free_connection(&tls);
        return -1;
    }

    int ret = SSL_connect(tls->ssl);
    if (ret != 1) {
        int err = SSL_get_error(tls->ssl, ret);
        ERROR_PRINT("SSL Connect was not successfull, error %i", err);
        ERROR_PRINT("%s", stderr);
        close(tcp_sock);
        TLS_free_connection(&tls);
        return -1;
    }

    INFO_PRINT("Going to write %s", write_buffer);
    SSL_write(tls->ssl, write_buffer, strlen(write_buffer));                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
    TLS_free_connection(&tls);
    close(tcp_sock);
    return 0;
}