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
void removeChar(char*, char);

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
                char tweet[1000] = {'\0'};
                char user_token[1000] = {'\0'};
                char *tweet_line = calloc(1000, 1);
                strcpy(tweet_line, buf);
                char *bio_token2 = strtok(tweet_line, delim);
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
                    strcat(tweet, bio_token2);
                    strcat(tweet, " ");
                }
                tweet[strlen(tweet) - 1] = '\0';
                printf("\ntweet: %s\n", tweet);

                char *last_tweet_filename = "Resources/last_tweet_number.txt";

                FILE *ltn = fopen(last_tweet_filename, "r");
                int last_tweet_number;
                fscanf(ltn, "%d", &last_tweet_number);
                fclose(ltn);

                int this_tweet_id = last_tweet_number + 1;

                char *this_user = get_user_by_token(user_token);

                printf("%s\n", this_user);

                char file_name[1000];
                sprintf(file_name, "Resources/Users/%s.user.json", this_user);

                char tweet_file_name[1000];
                sprintf(tweet_file_name, "Resources/Tweets/%d.tweet.json", this_tweet_id);

                cJSON *tweet_root;
                tweet_root = cJSON_CreateObject();

                cJSON_AddNumberToObject(tweet_root, "id", this_tweet_id);
                cJSON_AddStringToObject(tweet_root, "author", this_user);
                cJSON_AddStringToObject(tweet_root, "content", tweet);
                cJSON * comments = cJSON_CreateObject();
                cJSON_AddItemToObject(tweet_root, "comments", comments);
                cJSON_AddNumberToObject(tweet_root, "likes", 0);
                cJSON * users_liked = cJSON_CreateArray();
                cJSON_AddItemToObject(tweet_root, "users_liked", users_liked);

                printf("\n\n\n%s\n", cJSON_Print(tweet_root));

                char *tweet_string = cJSON_Print(tweet_root);
                FILE *tweet_file;
                tweet_file = fopen(tweet_file_name, "w");
                fprintf(tweet_file, "%s", tweet_string);
                fclose(tweet_file);



                FILE *fp = fopen(file_name, "r");
                char user_file_content[10000];
                char user_object[10000] = {'\0'};

                while (fgets(user_file_content, 10000, fp) != NULL)
                    strcat(user_object, user_file_content);

                fclose(fp);

                cJSON * user_data = cJSON_Parse(user_object);
                cJSON * tweet_id = cJSON_CreateNumber(this_tweet_id);
                cJSON_AddItemToArray(cJSON_GetObjectItem(user_data, "personalTweets"), tweet_id);
                char *user_string = cJSON_Print(user_data);
                FILE *user_file;
                user_file = fopen(file_name, "w");
                fprintf(user_file, "%s", user_string);
                fclose(user_file);

                FILE *last_tweet_file;
                last_tweet_file = fopen(last_tweet_filename, "w");
                fprintf(last_tweet_file, "%d", this_tweet_id);
                fclose(last_tweet_file);

                char *new_tweet_response = "{\"type\": \"Successful\",\"message\":\"\"}";
                int sent_status = sendData(&client_fd, &client, new_tweet_response);

            }
            else if (strcmp(command, "refresh") == 0) {
                char user_token[1010];
                char *login_line = calloc(1017, 1);
                strcpy(login_line, buf);
                char *login_token2 = strtok(login_line, delim);
                login_token2 = strtok(NULL, delim);
                strcpy(user_token, login_token2);

                char *this_user = get_user_by_token(user_token);

                char *user_filename = calloc(1000, 1);
                sprintf(user_filename, "Resources/Users/%s.user.json", this_user);


                FILE *fp = fopen(user_filename, "r");
                char user_file_content[10000];
                char user_object[10000] = {'\0'};

                while (fgets(user_file_content, 10000, fp) != NULL)
                    strcat(user_object, user_file_content);

                fclose(fp);

                cJSON * user_data = cJSON_Parse(user_object);

                cJSON *followings = cJSON_GetObjectItem(user_data, "followings");

                cJSON *all_tweets = cJSON_CreateArray();


                for (int i = 0; i < cJSON_GetArraySize(followings); i++) {
                    cJSON *subitem = cJSON_GetArrayItem(followings, i);
                    char *user = cJSON_GetStringValue(subitem);
                    char *this_user_filename = calloc(100, 1);
                    sprintf(this_user_filename, "Resources/Users/%s.user.json", user);
                    FILE *this_user_file = fopen(this_user_filename, "r");
                    char temp_user[10000];
                    char user_content[10000] = {'\0'};
                    while (fgets(temp_user, 10000, this_user_file) != NULL) {
                        strcat(user_content, temp_user);
                    }
                    fclose(fp);

                    cJSON *user_obj = cJSON_Parse(user_content);
                    cJSON *tweets = cJSON_GetObjectItem(user_obj, "personalTweets");

                    for (int j = 0; j < cJSON_GetArraySize(tweets); j++) {
                        cJSON *subitem2 = cJSON_GetArrayItem(tweets, j);
                        int tweet_id = (int) cJSON_GetNumberValue(subitem2);
                        char *this_tweet_filename = calloc(100, 1);
                        sprintf(this_tweet_filename, "Resources/Tweets/%d.tweet.json", tweet_id);
                        FILE *this_tweet_file = fopen(this_tweet_filename, "r");
                        char temp[10000];
                        char tweet_content[10000] = {'\0'};
                        while (fgets(temp, 10000, this_tweet_file) != NULL) {
                            strcat(tweet_content, temp);
                        }
                        fclose(fp);

                        cJSON *this_tweet = cJSON_Parse(tweet_content);

                        cJSON_AddItemToArray(all_tweets, this_tweet);

                    }
                }

                char *all_tweets_rendered = cJSON_Print(all_tweets);

                printf("\n%s\n", all_tweets_rendered);

                char *refresh_response = calloc(10000, 1);
                sprintf(refresh_response, "{\"type\": \"List\",\"message\":%s}", all_tweets_rendered);

                removeChar(refresh_response, '\n');
                removeChar(refresh_response, '\t');

                int sent_status = sendData(&client_fd, &client, refresh_response);
            }
            else if (strcmp(command, "like") == 0) {
                char user_token[1000];
                char tweet_id[1000];
                char *like_line = calloc(1000, 1);
                strcpy(like_line, buf);
                char *signup_token2 = strtok(like_line, delim);
                signup_token2 = strtok(NULL, delim);
                strcpy(user_token, signup_token2);
                user_token[strlen(user_token) - 1] = '\0';
                signup_token2 = strtok(NULL, delim);
                strcpy(tweet_id, signup_token2);
                tweet_id[strlen(tweet_id)] = '\0';

                char *this_user = get_user_by_token(user_token);


                char *tweet_file = calloc(1000, 1);
                sprintf(tweet_file, "Resources/Tweets/%s.tweet.json", tweet_id);


                FILE *fp = fopen(tweet_file, "r");
                char user_file_content[10000];
                char user_object[10000] = {'\0'};

                while (fgets(user_file_content, 10000, fp) != NULL)
                    strcat(user_object, user_file_content);

                fclose(fp);

                cJSON * tweet_data = cJSON_Parse(user_object);
                cJSON * likes_value = cJSON_GetObjectItem(tweet_data, "likes");
                cJSON * users_liked = cJSON_GetObjectItem(tweet_data, "users_liked");

                int this_user_liked = 0;
                for (int i = 0; i < cJSON_GetArraySize(users_liked); i++) {
                    cJSON *subitem = cJSON_GetArrayItem(users_liked, i);
                    char *user = cJSON_GetStringValue(subitem);
                    printf("%s, %s\n", user, this_user);
                    if (strcmp(user, this_user) == 0){
                        this_user_liked = 1;
                    }
                }

                char *like_response = calloc(100, 1);

                if (this_user_liked == 0) {
                    int likes = (int)cJSON_GetNumberValue(likes_value);
                    likes++;
                    cJSON_SetNumberValue(likes_value, likes);
                    cJSON * this_user_obj = cJSON_CreateString(this_user);
                    cJSON_AddItemToArray(users_liked, this_user_obj);
                    char *tweet_string = cJSON_Print(tweet_data);
                    FILE *user_file;
                    user_file = fopen(tweet_file, "w");
                    fprintf(user_file, "%s", tweet_string);
                    fclose(user_file);


                    sprintf(like_response, "{\"type\": \"List\",\"message\":\"Tweet %s successfully liked.\"}", tweet_id);
                }
                else {
                    sprintf(like_response, "{\"type\": \"List\",\"message\":\"You already liked this tweet.\"}");
                }


                int sent_status = sendData(&client_fd, &client, like_response);

            }
            else if (strcmp(command, "comment") == 0) {
                char user_token[1000];
                char tweet_id[1000];
                char comment_message[1000] = {'\0'};
                char *comment_line = calloc(1000, 1);
                strcpy(comment_line, buf);
                char *change_token = strtok(comment_line, delim);
                change_token = strtok(NULL, delim);
                strcpy(user_token, change_token);
                user_token[strlen(user_token) - 1] = '\0';
                change_token = strtok(NULL, delim);
                strcpy(tweet_id, change_token);
                tweet_id[strlen(tweet_id) - 1] = '\0';
                while(change_token != NULL){
                    change_token = strtok(NULL, delim);
                    if (change_token == NULL){
                        break;
                    }
                    strcat(comment_message, change_token);
                    strcat(comment_message, " ");
                }
                comment_message[strlen(comment_message) - 1] = '\0';
                printf("%s", comment_message);

                char *tweet_file = calloc(1000, 1);
                sprintf(tweet_file, "Resources/Tweets/%s.tweet.json", tweet_id);


                FILE *fp = fopen(tweet_file, "r");
                char temp[10000];
                char tweet_file_content[10000] = {'\0'};

                while (fgets(temp, 10000, fp) != NULL)
                    strcat(tweet_file_content, temp);

                fclose(fp);

                char *this_user = get_user_by_token(user_token);

                cJSON * tweet_data = cJSON_Parse(tweet_file_content);
                cJSON * comments_obj = cJSON_GetObjectItem(tweet_data, "comments");
                cJSON_AddStringToObject(comments_obj, this_user, comment_message);
                char *tweet_string = cJSON_Print(tweet_data);
                FILE *user_file;
                user_file = fopen(tweet_file, "w");
                fprintf(user_file, "%s", tweet_string);
                fclose(user_file);


                char *comment_response = calloc(100, 1);
                sprintf(comment_response, "{\"type\": \"List\",\"message\":\"You commented on tweet %s.\"}", tweet_id);

                int sent_status = sendData(&client_fd, &client, comment_response);
            }
            else if (strcmp(command, "search") == 0) {
                char search_user_token[1000];
                char username_for_search[1000];
                char *signup_line = calloc(1000, 1);
                strcpy(signup_line, buf);
                char *signup_token2 = strtok(signup_line, delim);
                signup_token2 = strtok(NULL, delim);
                strcpy(search_user_token, signup_token2);
                search_user_token[strlen(search_user_token) - 1] = '\0';
                signup_token2 = strtok(NULL, delim);
                strcpy(username_for_search, signup_token2);
                username_for_search[strlen(username_for_search)] = '\0';

                printf("\ntoken: %s\nusername: %s\n", search_user_token, username_for_search);

                char *searched_user_file_name = calloc(100, 1);
                sprintf(searched_user_file_name, "Resources/Users/%s.user.json", username_for_search);

                FILE *fp = fopen(searched_user_file_name, "r");

                if (fp != NULL) {
                    char user_file_content[10000];
                    char user_object[10000] = {'\0'};

                    while (fgets(user_file_content, 10000, fp) != NULL) {
                        strcat(user_object, user_file_content);
                    }

                    fclose(fp);

                    cJSON *user_data = cJSON_Parse(user_object);


                    cJSON *response_message = cJSON_CreateObject();

                    cJSON *searched_username_object = cJSON_GetObjectItemCaseSensitive(user_data, "username");
                    char *searched_username = cJSON_GetStringValue(searched_username_object);
                    cJSON_AddStringToObject(response_message, "username", searched_username);

                    cJSON *searched_bio_object = cJSON_GetObjectItemCaseSensitive(user_data, "bio");
                    char *searched_bio = cJSON_GetStringValue(searched_bio_object);
                    cJSON_AddStringToObject(response_message, "bio", searched_bio);

                    cJSON * followers_list = cJSON_GetObjectItem(user_data, "followers");
                    cJSON_AddNumberToObject(response_message, "numberOfFollowers", cJSON_GetArraySize(followers_list));

                    cJSON * followings_list = cJSON_GetObjectItem(user_data, "followings");
                    cJSON_AddNumberToObject(response_message, "numberOfFollowings", cJSON_GetArraySize(followings_list));

                    int i, follow_status = 0;
                    char *this_user = get_user_by_token(search_user_token);
                    for (i = 0; i < cJSON_GetArraySize(followers_list); i++) {
                        cJSON *follower = cJSON_GetArrayItem(followers_list, i);
                        char * follower_username = cJSON_GetStringValue(follower);
                        if (strcmp(follower_username, this_user) == 0) {
                            follow_status = 1;
                        }
                    }

                    if (follow_status) {
                        cJSON_AddStringToObject(response_message, "followStatus", "Followed");
                    }
                    else {
                        cJSON_AddStringToObject(response_message, "followStatus", "NotFollowed");
                    }




                    cJSON *personal_tweets_ids = cJSON_GetObjectItem(user_data, "personalTweets");

                    cJSON *personal_tweets = cJSON_CreateArray();

                    for (i = 0; i < cJSON_GetArraySize(personal_tweets_ids); i++) {
                        cJSON *subitem = cJSON_GetArrayItem(personal_tweets_ids, i);
                        int tweet_id = (int) cJSON_GetNumberValue(subitem);
                        char *this_tweet_filename = calloc(100, 1);
                        sprintf(this_tweet_filename, "Resources/Tweets/%d.tweet.json", tweet_id);
                        FILE *this_tweet_file = fopen(this_tweet_filename, "r");
                        char temp[10000];
                        char tweet_content[10000] = {'\0'};
                        while (fgets(temp, 10000, this_tweet_file) != NULL) {
                            strcat(tweet_content, temp);
                        }
                        fclose(fp);

                        cJSON *this_tweet = cJSON_Parse(tweet_content);

                        cJSON_AddItemToArray(personal_tweets, this_tweet);

                    }

                    cJSON_AddItemToObject(response_message, "allTweets", personal_tweets);

                    char *response_message_rendered = cJSON_Print(response_message);
                    removeChar(response_message_rendered, '\n');
                    removeChar(response_message_rendered, '\t');
                    char *search_response = calloc(100000, 1);
                    sprintf(search_response, "{\"type\": \"Profile\",\"message\":%s}", response_message_rendered);
                    printf("\n%s\n", search_response);
                    int sent_status = sendData(&client_fd, &client, search_response);
                }
                else {
                    char *search_response = "{\"type\": \"Error\",\"message\":\"User doesn't exists.\"}";
                    int sent_status = sendData(&client_fd, &client, search_response);
                }

            }
            else if (strcmp(command, "follow") == 0) {
                char user_token[1000];
                char intended_user[1000];
                char *follow_line = calloc(1000, 1);
                strcpy(follow_line, buf);
                char *follow_token = strtok(follow_line, delim);
                follow_token = strtok(NULL, delim);
                strcpy(user_token, follow_token);
                user_token[strlen(user_token) - 1] = '\0';
                follow_token = strtok(NULL, delim);
                strcpy(intended_user, follow_token);
                intended_user[strlen(intended_user)] = '\0';

                char *this_user = get_user_by_token(user_token);

                char *this_user_filename = calloc(1000, 1);
                sprintf(this_user_filename, "Resources/Users/%s.user.json", this_user);

                FILE *fp = fopen(this_user_filename, "r");
                char this_user_file_content[10000];
                char this_user_object[10000] = {'\0'};

                while (fgets(this_user_file_content, 10000, fp) != NULL)
                    strcat(this_user_object, this_user_file_content);

                fclose(fp);

                cJSON * user_data = cJSON_Parse(this_user_object);
                cJSON * followings = cJSON_GetObjectItem(user_data, "followings");
                cJSON *intended_user_obj = cJSON_CreateString(intended_user);
                cJSON_AddItemToArray(followings, intended_user_obj);
                char *user_string = cJSON_Print(user_data);
                FILE *this_user_file;
                this_user_file = fopen(this_user_filename, "w");
                fprintf(this_user_file, "%s", user_string);
                fclose(this_user_file);

                char *intended_user_filename = calloc(1000, 1);
                sprintf(intended_user_filename, "Resources/Users/%s.user.json", intended_user);

                FILE *intendet_user_file_r = fopen(intended_user_filename, "r");
                char intended_user_file_content[10000];
                char intended_user_object[10000] = {'\0'};

                while (fgets(intended_user_file_content, 10000, intendet_user_file_r) != NULL)
                    strcat(intended_user_object, intended_user_file_content);

                fclose(fp);

                cJSON * intended_user_data = cJSON_Parse(intended_user_object);
                cJSON * followers = cJSON_GetObjectItem(intended_user_data, "followers");
                cJSON *this_user_obj = cJSON_CreateString(this_user);
                cJSON_AddItemToArray(followers, this_user_obj);
                char *intended_user_string = cJSON_Print(intended_user_data);
                FILE *intended_user_file;
                intended_user_file = fopen(intended_user_filename, "w");
                fprintf(intended_user_file, "%s", intended_user_string);
                fclose(intended_user_file);

                free(this_user_file_content);
                free(this_user_object);
                free(intended_user_file_content);
                free(intended_user_object);

                char *follow_response = calloc(1000, 1);
                sprintf(follow_response, "{\"type\": \"Successful\",\"message\":\"You are now following %\"}", intended_user);
                int sent_status = sendData(&client_fd, &client, follow_response);
            }
            else if (strcmp(command, "unfollow") == 0) {
                char user_token[1000];
                char intended_user[1000];
                char *unfollow_line = calloc(1000, 1);
                strcpy(unfollow_line, buf);
                char *unfollow_token = strtok(unfollow_line, delim);
                unfollow_token = strtok(NULL, delim);
                strcpy(user_token, unfollow_token);
                user_token[strlen(user_token) - 1] = '\0';
                unfollow_token = strtok(NULL, delim);
                strcpy(intended_user, unfollow_token);
                intended_user[strlen(intended_user)] = '\0';

                char *this_user = get_user_by_token(user_token);

                char *this_user_filename = calloc(1000, 1);
                sprintf(this_user_filename, "Resources/Users/%s.user.json", this_user);

                FILE *fp = fopen(this_user_filename, "r");
                char this_user_file_content[10000];
                char this_user_object[10000] = {'\0'};


                while (fgets(this_user_file_content, 10000, fp) != NULL)
                    strcat(this_user_object, this_user_file_content);

                fclose(fp);

                cJSON * user_data = cJSON_Parse(this_user_object);
                cJSON * followings = cJSON_GetObjectItem(user_data, "followings");

                int i = 0;
                for (i = 0; i < cJSON_GetArraySize(followings); i++) {
                    cJSON *following = cJSON_GetArrayItem(followings, i);
                    char * following_username = cJSON_GetStringValue(following);
                    if (strcmp(following_username, intended_user) == 0) {
                         break;
                    }
                }
                printf("\n%d", i);
                cJSON_DeleteItemFromArray(followings, i);

                char *user_string = cJSON_Print(user_data);
                FILE *this_user_file;
                this_user_file = fopen(this_user_filename, "w");
                fprintf(this_user_file, "%s", user_string);
                fclose(this_user_file);

                char *intended_user_filename = calloc(1000, 1);
                sprintf(intended_user_filename, "Resources/Users/%s.user.json", intended_user);

                FILE *intendet_user_file_r = fopen(intended_user_filename, "r");
                char intended_user_file_content[10000];
                char intended_user_object[10000] = {'\0'};

                while (fgets(intended_user_file_content, 10000, intendet_user_file_r) != NULL)
                    strcat(intended_user_object, intended_user_file_content);

                fclose(fp);

                cJSON * intended_user_data = cJSON_Parse(intended_user_object);
                cJSON * followers = cJSON_GetObjectItem(intended_user_data, "followers");

                for (i = 0; i < cJSON_GetArraySize(followers); i++) {
                    cJSON *follower = cJSON_GetArrayItem(followers, i);
                    char * follower_username = cJSON_GetStringValue(follower);
                    if (strcmp(follower_username, this_user) == 0) {
                        break;
                    }
                }
                cJSON_DeleteItemFromArray(followers, i);

                char *intended_user_string = cJSON_Print(intended_user_data);
                FILE *intended_user_file;
                intended_user_file = fopen(intended_user_filename, "w");
                fprintf(intended_user_file, "%s", intended_user_string);
                fclose(intended_user_file);

                free(this_user_file_content);
                free(this_user_object);
                free(intended_user_file_content);
                free(intended_user_object);

                char *unfollow_response = calloc(1000, 1);
                sprintf(unfollow_response, "{\"type\": \"Successful\",\"message\":\"You are now following %s\"}", intended_user);
                int sent_status = sendData(&client_fd, &client, unfollow_response);
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
                char profile_user_token[1000];
                char *signup_line = calloc(1000, 1);
                strcpy(signup_line, buf);
                char *signup_token2 = strtok(signup_line, delim);
                signup_token2 = strtok(NULL, delim);
                strcpy(profile_user_token, signup_token2);
                profile_user_token[strlen(profile_user_token)] = '\0';

                char *searched_user_file_name = calloc(100, 1);
                char *this_user = get_user_by_token(profile_user_token);
                sprintf(searched_user_file_name, "Resources/Users/%s.user.json", this_user);

                FILE *fp = fopen(searched_user_file_name, "r");

                if (fp != NULL) {
                    char user_file_content[10000];
                    char user_object[10000] = {'\0'};

                    while (fgets(user_file_content, 10000, fp) != NULL) {
                        strcat(user_object, user_file_content);
                    }

                    fclose(fp);

                    cJSON *user_data = cJSON_Parse(user_object);


                    cJSON *response_message = cJSON_CreateObject();

                    cJSON *searched_username_object = cJSON_GetObjectItemCaseSensitive(user_data, "username");
                    char *searched_username = cJSON_GetStringValue(searched_username_object);
                    cJSON_AddStringToObject(response_message, "username", searched_username);

                    cJSON *searched_bio_object = cJSON_GetObjectItemCaseSensitive(user_data, "bio");
                    char *searched_bio = cJSON_GetStringValue(searched_bio_object);
                    cJSON_AddStringToObject(response_message, "bio", searched_bio);

                    cJSON * followers_list = cJSON_GetObjectItem(user_data, "followers");
                    cJSON_AddNumberToObject(response_message, "numberOfFollowers", cJSON_GetArraySize(followers_list));

                    cJSON * followings_list = cJSON_GetObjectItem(user_data, "followings");
                    cJSON_AddNumberToObject(response_message, "numberOfFollowings", cJSON_GetArraySize(followings_list));


                    cJSON *personal_tweets_ids = cJSON_GetObjectItem(user_data, "personalTweets");

                    cJSON *personal_tweets = cJSON_CreateArray();

                    for (int i = 0; i < cJSON_GetArraySize(personal_tweets_ids); i++) {
                        cJSON *subitem = cJSON_GetArrayItem(personal_tweets_ids, i);
                        int tweet_id = (int) cJSON_GetNumberValue(subitem);
                        char *this_tweet_filename = calloc(100, 1);
                        sprintf(this_tweet_filename, "Resources/Tweets/%d.tweet.json", tweet_id);
                        FILE *this_tweet_file = fopen(this_tweet_filename, "r");
                        char temp[10000];
                        char tweet_content[10000] = {'\0'};
                        while (fgets(temp, 10000, this_tweet_file) != NULL) {
                            strcat(tweet_content, temp);
                        }
                        fclose(fp);

                        cJSON *this_tweet = cJSON_Parse(tweet_content);

                        cJSON_AddItemToArray(personal_tweets, this_tweet);

                    }

                    cJSON_AddItemToObject(response_message, "allTweets", personal_tweets);

                    char *response_message_rendered = cJSON_Print(response_message);
                    removeChar(response_message_rendered, '\n');
                    removeChar(response_message_rendered, '\t');
                    char *search_response = calloc(100000, 1);
                    sprintf(search_response, "{\"type\": \"Profile\",\"message\":%s}", response_message_rendered);
                    printf("\n%s\n", search_response);
                    int sent_status = sendData(&client_fd, &client, search_response);
                }
                else {
                    char *search_response = "{\"type\": \"Error\",\"message\":\"Fot.\"}";
                    int sent_status = sendData(&client_fd, &client, search_response);
                }
            }
            else if (strcmp(command, "change") == 0) {
                char user_token[1000];
                char current_password[1000];
                char new_password[1000];
                char *chnage_passsword_line = calloc(1000, 1);
                strcpy(chnage_passsword_line, buf);
                char *change_token = strtok(chnage_passsword_line, delim);
                change_token = strtok(NULL, delim);
                change_token = strtok(NULL, delim);
                strcpy(user_token, change_token);
                user_token[strlen(user_token) - 1] = '\0';
                change_token = strtok(NULL, delim);
                strcpy(current_password, change_token);
                current_password[strlen(current_password) - 1] = '\0';
                change_token = strtok(NULL, delim);
                strcpy(new_password, change_token);
                new_password[strlen(new_password)] = '\0';

                char *this_user_file_name = calloc(100, 1);
                char *this_user = get_user_by_token(user_token);
                sprintf(this_user_file_name, "Resources/Users/%s.user.json", this_user);

                FILE *fp = fopen(this_user_file_name, "r");

                char user_file_content[10000];
                char user_object[10000] = {'\0'};

                while (fgets(user_file_content, 10000, fp) != NULL) {
                    strcat(user_object, user_file_content);
                }

                fclose(fp);

                cJSON *user_data = cJSON_Parse(user_object);
                cJSON *this_password_obj = cJSON_GetObjectItem(user_data, "password");
                char *this_password = cJSON_GetStringValue(this_password_obj);

                if (strcmp(this_password, current_password) == 0) {
                    cJSON_SetValuestring(this_password_obj, new_password);
                    char *user_string = cJSON_Print(user_data);
                    FILE *user_file;
                    user_file = fopen(this_user_file_name, "w");
                    fprintf(user_file, "%s", user_string);
                    fclose(user_file);

                    char *change_response = "{\"type\": \"Successful\",\"message\":\"Password has changed successfully\"}";
                    int sent_status = sendData(&client_fd, &client, change_response);
                }
                else {
                    char *change_response = "{\"type\": \"Error\",\"message\":\"Current password is incorrect.\"}";
                    int sent_status = sendData(&client_fd, &client, change_response);
                }
            }


            free(buf);
        }
        while (keepLooping);

        closesocket(client_fd);
    }

    WSACleanup();
    return 0;
}

int sendData(SOCKET *client_fd, struct sockaddr_in *client, char *response) {
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

void print_logins() {
    struct Login *ptr = head;
    while (ptr != NULL) {
        printf("%s:%s\n", ptr->username, ptr->token);
        ptr = ptr->next;
    }
}

void removeChar(char* s, char c) {

    int j, n = strlen(s);
    for (int i = j = 0; i < n; i++)
        if (s[i] != c)
            s[j++] = s[i];

    s[j] = '\0';
}