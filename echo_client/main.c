#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#define PORT 12345


int initialize(int);
char *send_data(char *);


struct sockaddr_in server;


int main() {
    char line[100];
    int can_initialize = initialize(PORT);
    char *buf;

    while (can_initialize == 0) {
        gets(line);
        printf("%s\n", line);

        buf = send_data(line);
    }
}


int initialize(int port) {
    WSADATA wsadata;
    int wsaStartUp = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (wsaStartUp != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return -1;
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(port);
    return 0;
}

char *send_data(char *data) {
    char *buffer = malloc(1000);
    memset(buffer, 0, 1000);
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        return buffer;
    }
    int can_connect = connect(client_socket, (struct sockaddr *) &server, sizeof(server));
    if (can_connect != 0) {
        printf("connection failed");
        return buffer;
    }
    send(client_socket, data, strlen(data), 0);
    recv(client_socket, buffer, 999, 0);
    printf("%s", buffer);
    closesocket(client_socket);
    return buffer;
}
