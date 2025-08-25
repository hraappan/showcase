
#include "logging.h"
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

static const char* ip = "192.168.1.137";
static const uint16_t port = 6666;

int main(int argc, char *argv[])
{
    struct sockaddr_in sockaddr;
    socklen_t server_len = sizeof(sockaddr);

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
    
    if (connect(tcp_sock, (struct sockaddr *) &sockaddr, server_len) < 0) {
        ERROR_PRINT("Cannot connect to server ip %s, errno %i", ip, errno);
        close(tcp_sock);
        return -1;
    }

    close(tcp_sock);
    return 0;
}