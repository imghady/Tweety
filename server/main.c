#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "cJSON.h"
#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#define PORT 12345
#define BUFFER_SIZE 1024

struct sockaddr_in server;

int initialize(int);
char *sendToClient(char *);
int sendData(SOCKET *, struct sockaddr_in *, char *);
void remove_doublequot (char * input);
void rand_str(char *, size_t);
void add_new_login(char *, char *);
void delete_login(char *);
char * get_user_by_token(char *);
void print_logins();

struct Login {
    char username[1000];
    char token[1010];
    struct Login * next;
};
struct Login *head = NULL;

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
            printf("command is: %s\n", buf);

            strcpy(arguments, buf);


            const char delim[2] = " ";
            char *token = strtok(arguments, delim);
            strcpy(command, token);

            buf[strlen(buf) - 1] = '\0';

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

                char *login_response = calloc(1000, 1);

                char file_name[1000];
                sprintf(file_name, "Resources/Users/%s.user.json", login_username);

                FILE *login_check_file_existence;
                if ((login_check_file_existence = fopen(file_name, "r"))){
                    char user_file_content[10000];
                    char user_object[10000] = {'\0'};

                    while (fgets(user_file_content, 10000, login_check_file_existence) != NULL)
                        strcat(user_object, user_file_content);

                    printf("user object is: x%s", user_object);

                    cJSON * user_data = cJSON_Parse(user_object);
                    cJSON * real_password = cJSON_GetObjectItemCaseSensitive(user_data, "password");
                    char * real_password_rendered = cJSON_Print(real_password);

                    remove_doublequot(real_password_rendered);

                    printf("\nrendered password: %s\n", real_password_rendered);

                    if (strcmp(real_password_rendered, login_password) == 0) {
                        char *res_token = calloc(100, 1);
                        char random_string[10];
                        strcpy(res_token, login_username);
                        rand_str(random_string, 10);
                        strcat(res_token, random_string);

                        printf("token for this login: %s\n", res_token);

                        add_new_login(login_username, res_token);
                        print_logins();
                        sprintf(login_response, "{\"type\":\"Token\",\"message\":\"%s\"}", res_token);
                    }
                    else {
                        login_response = "{\"type\":\"Error\",\"message\":\"Incorrect password\"}";
                    }

                    user_file_content[0] = '\0';
                    user_object[0] = '\0';

                    fclose(login_check_file_existence);
                }
                else {
                    fclose(login_check_file_existence);
                    login_response = "{\"type\":\"Error\",\"message\":\"Invalid username\"}";
                }


                int sent_status = sendData(&client_fd, &client, login_response);

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
                signup_password[strlen(signup_password)] = '\0';

                cJSON *followers_field = NULL;
                followers_field = cJSON_CreateArray();

                cJSON *followings_field = NULL;
                followings_field = cJSON_CreateArray();

                cJSON *personalTweets_field = NULL;
                personalTweets_field = cJSON_CreateArray();


                printf("\n%s:%s\n", signup_username, signup_password);

                cJSON_AddStringToObject(signup_root, "username", signup_username);
                cJSON_AddStringToObject(signup_root, "password", signup_password);
                cJSON_AddStringToObject(signup_root, "bio", "");

                cJSON_AddItemToObject(signup_root, "followers", followers_field);
                cJSON_AddItemToObject(signup_root, "followings", followings_field);
                cJSON_AddItemToObject(signup_root, "personalTweets", personalTweets_field);

                char *signup_string = cJSON_Print(signup_root);
                char *signup_response = calloc(1000, 1);

                char signup_file_name[1000];
                sprintf(signup_file_name, "Resources/Users/%s.user.json", signup_username);

                FILE *signup_check_file_existence;
                if ((signup_check_file_existence = fopen(signup_file_name, "r"))){
                    fclose(signup_check_file_existence);
                    signup_response = "{\"type\": \"Error\",\"message\":\"This user is already taken.\"}";
                }
                else {
                    FILE *user_file;
                    printf("\n%s\n", signup_file_name);
                    user_file = fopen(signup_file_name, "w");
                    fprintf(user_file, "%s", signup_string);
                    fclose(user_file);
                    signup_response = "{\"type\":\"Successful\",\"message\":\"\"}";
                }

                int sent_status = sendData(&client_fd, &client, signup_response);

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
                char bio[1000] = {'\0'};
                char user_token[1000] = {'\0'};
                char *bio_line = calloc(1000, 1);
                strcpy(bio_line, buf);
                char *bio_token2 = strtok(bio_line, delim);
                bio_token2 = strtok(NULL, delim);
                bio_token2 = strtok(NULL, delim);
                strcpy(user_token, bio_token2);
                user_token[strlen(user_token) - 1] = '\0';
                printf("\ntoken: %s", user_token);
                while(bio_token2 != NULL){
                    bio_token2 = strtok(NULL, delim);
                    if (bio_token2 == NULL){
                        break;
                    }
                    strcat(bio, bio_token2);
                    strcat(bio, " ");
                }
                bio[strlen(bio) - 1] = '\0';
                printf("\nbio: %s\n", bio);

                char *this_user = get_user_by_token(user_token);

                printf("%s\n", this_user);

                char file_name[1000];
                sprintf(file_name, "Resources/Users/%s.user.json", this_user);

                FILE *fp = fopen(file_name, "r");
                char user_file_content[10000];
                char user_object[10000] = {'\0'};

                while (fgets(user_file_content, 10000, fp) != NULL)
                    strcat(user_object, user_file_content);

                fclose(fp);

                cJSON * user_data = cJSON_Parse(user_object);
                cJSON_SetValuestring(cJSON_GetObjectItem(user_data, "bio"), bio);
                char *signup_string = cJSON_Print(user_data);
                FILE *user_file;
                user_file = fopen(file_name, "w");
                fprintf(user_file, "%s", signup_string);
                fclose(user_file);

                char *logout_response = "{\"type\": \"Successful\",\"message\":\"\"}";
                int sent_status = sendData(&client_fd, &client, logout_response);

            }
            else if (strcmp(command, "logout") == 0) {
                char user_token[1010];
                char *login_line = calloc(1017, 1);
                strcpy(login_line, buf);
                char *login_token2 = strtok(login_line, delim);
                login_token2 = strtok(NULL, delim);
                strcpy(user_token, login_token2);
                printf("\n%s\n", user_token);
                delete_login(user_token);
                print_logins();
                char *logout_response = "{\"type\": \"Successful\",\"message\":\"\"}";
                int sent_status = sendData(&client_fd, &client, logout_response);
            }
            else if (strcmp(command, "profile") == 0) {

            }
            else if (strcmp(command, "change") == 0) {

            }


            free(buf);
        }
        while (keepLooping);

        closesocket(client_fd);
    }

    WSACleanup();
    return 0;
}

int sendData(SOCKET *client_fd, struct sockaddr_in *client, char *response){
    unsigned long long resp_len = strlen(response);

    while (resp_len > 0){
        int sent = send(*client_fd, response, (int)resp_len, 0);

        if (sent == SOCKET_ERROR){
            return -1;
        }
        response += sent;
        resp_len -= sent;
    }

}

void remove_doublequot (char * input) {
    int i = 0;
    while (*(input + i) != NULL){
        *(input + i) = *(input + i + 1);
        i++;
    }
    *(input + i - 2) = '\0';
}

void rand_str(char *dest, size_t length) {
    char charset[] = "0123456789"
                     "abcdefghijklmnopqrstuvwxyz"
                     "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    while (length-- > 0) {

        size_t index = (double) rand() / RAND_MAX * (sizeof charset - 1);
        *dest++ = charset[index];
    }
    *dest = '\0';
}

void add_new_login(char *username, char *token) {
    struct Login *link = (struct Login *) malloc(sizeof(struct Login));
    strcpy(link->username, username);
    strcpy(link->token, token);
    link->next = head;
    head = link;
}

void delete_login(char *token) {
    struct Login *current = head;
    struct Login *previous = NULL;
    while (strcmp(current->token, token) != 0) {
        if (current->next == NULL) {
            break;
        } else {
            previous = current;
            current = current->next;
        }
    }
    if (current == head) {
        head = head->next;
    } else {
        previous->next = current->next;
    }

    free(current);
}

char * get_user_by_token(char *token) {
    struct Login *current = head;
    if (head == NULL) {
        return NULL;
    }

    while (strcmp(current->token, token) != 0) {
        if (current->next == NULL) {
            break;
        } else {
            current = current->next;
        }
    }

    if (strcmp(current->token, token) == 0){
        return current->username;
    }
    else {
        return NULL;
    }
}

void print_logins(){
    struct Login *ptr = head;
    while (ptr != NULL) {
        printf("%s:%s\n", ptr->username, ptr->token);
        ptr = ptr->next;
    }
}