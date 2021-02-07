#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "cJSON.h"
#include <stdbool.h>
#define PORT 12345
#define BUFFER_SIZE 1024

struct sockaddr_in server;

int initialize(int);
char *sendToClient(char *);

int main() {
    WSADATA wsadata;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server, client;
    int port = PORT, err;
    char command[20];
    char *arguments = calloc(100, 1);

    err = WSAStartup(MAKEWORD(2,2), &wsadata);
    if (err != 0)
        printf("Error in WSAStartup", &err);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == INVALID_SOCKET)
        printf("Failed to initialize socket");

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = INADDR_ANY;

    /** bind & listen **/
    const BOOL opt_val = TRUE;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt_val, sizeof(opt_val));
    err = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
    if (err == SOCKET_ERROR)
        printf("Non ho potuto fare il bind del socket");
    err = listen(server_fd, 1);
    if (err == SOCKET_ERROR)
        printf("Non ho potuto mettermi in ascolto sul socket");

    printf("SERVER LISTENING ON PORT %d\n", port);

    while (1)
    {
        int client_len = sizeof(client);
        client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

        if (client_fd == INVALID_SOCKET)
            printf("Non riesco a stabilire una nuova connessione");

        bool keepLooping = true;
        do
        {
            char *buf = calloc(100, 1);
            int read = recv(client_fd, buf, BUFFER_SIZE, 0);

            if (read == 0)
                break;

            if (read == SOCKET_ERROR)
            {
                err = WSAGetLastError();
                if ((err != WSAENOTCONN) && (err != WSAECONNABORTED) && (err == WSAECONNRESET))
                    printf("Error nella lettura dal client", &err);
                break;
            }
            printf("%s\n", buf);

            strcpy(arguments, buf);


            const char delim[2] = " ";
            char *token = strtok(arguments, delim);
            strcpy(command, token);

            if (strcmp(command, "login") == 0){
                char login_username[1000];
                char login_password[1000];
                char *login_line = calloc(1000, 1);
                strcpy(login_line, buf);
                char *login_token2 = strtok(login_line, delim);
                login_token2 = strtok(NULL, delim);
                strcpy(login_username, login_token2);
                login_username[strlen(login_username) - 1] = '\0';
                login_token2 = strtok(NULL, delim);
                strcpy(login_password, login_token2);

                printf("\n%s:%s\n", login_username, login_password);

            }
            else if (strcmp(command, "signup") == 0) {
                cJSON *signup_root;
                signup_root = cJSON_CreateObject();
                char signup_username[1000];
                char signup_password[1000];
                char *signup_line = calloc(1000, 1);
                strcpy(signup_line, buf);
                char *signup_token2 = strtok(signup_line, delim);
                signup_token2 = strtok(NULL, delim);
                strcpy(signup_username, signup_token2);
                signup_username[strlen(signup_username) - 1] = '\0';
                signup_token2 = strtok(NULL, delim);
                strcpy(signup_password, signup_token2);

                printf("\n%s:%s\n", signup_username, signup_password);

                cJSON_AddStringToObject(signup_root, "username", signup_username);
                cJSON_AddStringToObject(signup_root, "password", signup_password);
                cJSON_AddStringToObject(signup_root, "bio", "");

                char *signup_string = cJSON_Print(signup_root);

                FILE *user_file;
                char file_name[1000];
                sprintf(file_name, "Resources/Users/%s.user.json", signup_username);
                printf("%s", file_name);
                user_file = fopen(file_name, "w");
                fprintf(user_file, "%s", signup_string);
                fclose(user_file);

            }
            else if (strcmp(command, "send") == 0) {
                char tweet[1000];

            }
            else if (strcmp(command, "refresh") == 0) {

            }
            else if (strcmp(command, "like") == 0) {

            }
            else if (strcmp(command, "comment") == 0) {

            }
            else if (strcmp(command, "search") == 0) {

            }
            else if (strcmp(command, "follow") == 0) {

            }
            else if (strcmp(command, "unfollow") == 0) {

            }
            else if (strcmp(command, "set") == 0) {
                char bio[1000];
                char bio_set[1000];
                char *bio_line = calloc(1000, 1);
                strcpy(bio_line, buf);
                char *bio_token2 = strtok(bio_line, delim);
                bio_token2 = strtok(NULL, delim);
                strcpy(bio_set, bio_token2);
                bio_set[strlen(bio_set) - 1] = '\0';
                bio_token2 = strtok(NULL, delim);
                strcpy(bio, bio_token2);
                printf("\n%s\n", bio);
            }
            else if (strcmp(command, "logout") == 0) {

            }
            else if (strcmp(command, "profile") == 0) {

            }
            else if (strcmp(command, "change") == 0) {

            }



            char *pbuf = buf;
            do
            {
                int sent = send(client_fd, pbuf, read, 0);
                if (sent == SOCKET_ERROR)
                {
                    err = WSAGetLastError();
                    if ((err != WSAENOTCONN) && (err != WSAECONNABORTED) && (err == WSAECONNRESET))
                        printf("Errore nella scrittura verso il client", &err);

                    keepLooping = false;
                    break;
                }

                pbuf += sent;
                read -= sent;
            }
            while (read > 0);
            free(buf);
        }
        while (keepLooping);

        closesocket(client_fd);
    }

    WSACleanup();
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
    closesocket(client_socket);
    return buffer;
}