#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "cJSON.h"
#define PORT 12345


int initialize(int);
void print_new();
char *send_data(char *);

void print_register_menu ();
void print_main_menu ();
void print_timeline ();
void print_Personal();
int confirm_password ( char password[], char c_password[] );
void remove_doublequot (char * input);
struct sockaddr_in server;
int m_index = 0;

int main() {
    char middle[] = ", ";
    int can_initialize = initialize(PORT);
    while (can_initialize == 0) {
        print_register_menu();
        int key;
        scanf("%d", &key);
        if (key == 1) {
            char username[1000];
            char password[1000];
            printf("enter your username:\n");
            scanf("%s", username);
            printf("enter your password:\n");
            scanf("%s",password);
            char * login_feedback;
            char to_send_login[1000] = "login ";
            strcat(to_send_login, username);
            strcat(to_send_login, middle);
            strcat(to_send_login, password);
            strcat(to_send_login, "\n");
            login_feedback = send_data(to_send_login);
            //printf("%s\n", login_feedback);
            cJSON * root = cJSON_Parse(login_feedback);
            cJSON *message = cJSON_GetObjectItemCaseSensitive(root, "message");
            cJSON *type = cJSON_GetObjectItemCaseSensitive(root, "type");
            char * type_rendered = cJSON_Print(type);
            remove_doublequot(type_rendered);
            //printf("\n%s\n", type_rendered);
            char * message_rendered = cJSON_Print(message);
            remove_doublequot(message_rendered);
            char *token= message_rendered;
            //printf("\n%s\n", token);
            if (strcmp(type_rendered, "Token") == 0) {
                printf("\nlogin successfully\n");

                while (1){
                    print_main_menu();
                    int command;
                    scanf("%d", &command);
                    if (command == 6) {
                        break;
                    }

                    else if (command == 1) {
                        while (1) {
                            int timeline_key;
                            print_timeline();
                            scanf("%d", &timeline_key);
                            if (timeline_key == 3) {
                                break;
                            }
                            else if (timeline_key == 1) {
                                printf("write your tweet:\n");
                                char tweet[1000];
                                scanf("%*c%[^\n]s", tweet);
                                //printf("\n%s\n", tweet);
                                //printf("\n%s\n", token);
                                char * tweet_feedback;
                                char to_send_tweet[1000] = "send tweet ";
                                strcat(to_send_tweet, token);
                                strcat(to_send_tweet, middle);
                                strcat(to_send_tweet, tweet);
                                strcat(to_send_tweet, "\n");
                                tweet_feedback = send_data(to_send_tweet);
                                //printf("%s\n", tweet_feedback);
                                cJSON * tweet_root = cJSON_Parse(tweet_feedback);
                                cJSON * tweet_type = cJSON_GetObjectItemCaseSensitive(tweet_root, "type");
                                char * tweet_type_rendered = cJSON_Print(tweet_type);
                                remove_doublequot(tweet_type_rendered);
                                if (strcmp(tweet_type_rendered, "Successful") == 0) {
                                    printf("\nyour tweet sent successfully.\n");
                                }

                            }
                            else if (timeline_key == 2) {

                            }


                        }
                    }

                    else if (command == 2) {

                    }

                    else if (command == 3) {
                        //printf("\n%s\n", token);
                        char * profile_feedback;
                        char to_send_profile[1000] = "profile ";
                        strcat(to_send_profile, token);
                        strcat(to_send_profile, "\n");
                        profile_feedback = send_data(to_send_profile);
                        //printf("%s\n", profile_feedback);

                        cJSON * root_profile = cJSON_Parse(profile_feedback);
                        cJSON * profile_message = cJSON_GetObjectItemCaseSensitive(root_profile, "message");
                        char * profile_message_rendered = cJSON_Print(profile_message);
                        //printf("\n%s\n", profile_message_rendered);

                        cJSON * root_username = cJSON_Parse(profile_message_rendered);
                        cJSON * profile_username = cJSON_GetObjectItemCaseSensitive(root_username, "username");
                        char * profile_username_rendered = cJSON_Print(profile_username);
                        remove_doublequot(profile_username_rendered);

                        cJSON * root_bio_profile = cJSON_Parse(profile_message_rendered);
                        cJSON * profile_bio = cJSON_GetObjectItemCaseSensitive(root_bio_profile, "bio");
                        char * profile_bio_rendered = cJSON_Print(profile_bio);
                        remove_doublequot(profile_bio_rendered);

                        cJSON * root_followers_profile = cJSON_Parse(profile_message_rendered);
                        cJSON * profile_followers = cJSON_GetObjectItemCaseSensitive(root_followers_profile, "numberOfFollowers");
                        char * profile_followers_rendered = cJSON_Print(profile_followers);

                        cJSON * root_followings_profile = cJSON_Parse(profile_message_rendered);
                        cJSON * profile_followings = cJSON_GetObjectItemCaseSensitive(root_followings_profile, "numberOfFollowings");
                        char * profile_followings_rendered = cJSON_Print(profile_followings);

                        cJSON * root_tweets_profile = cJSON_Parse(profile_message_rendered);
                        cJSON * profile_tweets = cJSON_GetObjectItemCaseSensitive(root_tweets_profile, "allTweets");
                        char * profile_tweets_rendered = cJSON_Print(profile_tweets);
                        int number_of_tweets = cJSON_GetArraySize(profile_tweets);


                        printf("\nusername: %s\n", profile_username_rendered);
                        printf("\nbio: %s\n", profile_bio_rendered);
                        printf("\nfollowers: %s\n", profile_followers_rendered);
                        printf("\nfollowings: %s\n", profile_followings_rendered);
                        //printf("\nall tweets: %s\n", profile_tweets_rendered);
                        printf("\nnumber of  tweets: %d\n", number_of_tweets);
                        printf("-------------------------------------------");

                        for (int k = 0; k < number_of_tweets; k++) {
                            cJSON * each_tweet = cJSON_GetArrayItem(profile_tweets, k);
                            char * each_tweet_rendered = cJSON_Print(each_tweet);
                            //printf("\ntweet %d is: %s\n", k, each_tweet_rendered);

                            cJSON * root_tweets_id_profile = cJSON_Parse(each_tweet_rendered);
                            cJSON * profile_tweets_id = cJSON_GetObjectItemCaseSensitive(root_tweets_id_profile, "id");
                            char * profile_tweets_id_rendered = cJSON_Print(profile_tweets_id);

                            cJSON * root_tweets_author_profile = cJSON_Parse(each_tweet_rendered);
                            cJSON * profile_tweets_author = cJSON_GetObjectItemCaseSensitive(root_tweets_author_profile, "author");
                            char * profile_tweets_author_rendered = cJSON_Print(profile_tweets_author);
                            remove_doublequot(profile_tweets_author_rendered);

                            cJSON * root_tweets_content_profile = cJSON_Parse(each_tweet_rendered);
                            cJSON * profile_tweets_content = cJSON_GetObjectItemCaseSensitive(root_tweets_content_profile, "content");
                            char * profile_tweets_content_rendered = cJSON_Print(profile_tweets_content);
                            remove_doublequot(profile_tweets_content_rendered);

                            cJSON * root_tweets_likes_profile = cJSON_Parse(each_tweet_rendered);
                            cJSON * profile_tweets_likes = cJSON_GetObjectItemCaseSensitive(root_tweets_likes_profile, "likes");
                            char * profile_tweets_likes_rendered = cJSON_Print(profile_tweets_likes);

                            printf("\nid: %s\n", profile_tweets_id_rendered);
                            printf("\nauthor: %s\n", profile_tweets_author_rendered);
                            printf("\ncontent: %s\n", profile_tweets_content_rendered);
                            printf("\nlikes: %s\n", profile_tweets_likes_rendered);
                            printf("-------------------------------------------");
                        }

                    }

                    else if (command == 4) {
                        while (1) {
                            int personal_key;
                            print_Personal();
                            scanf("%d", &personal_key);
                            if (personal_key == 3) {
                                break;
                            }
                            else if (personal_key == 1) {
                                printf("write your bio:\n");
                                char bio[1000];
                                scanf("%*c%[^\n]s", bio);
                                //printf("\n%s\n", bio);
                                //printf("\n%s\n", token);
                                char * bio_feedback;
                                char to_send_bio[1000] = "set bio ";
                                strcat(to_send_bio, token);
                                strcat(to_send_bio, middle);
                                strcat(to_send_bio, bio);
                                strcat(to_send_bio, "\n");
                                bio_feedback = send_data(to_send_bio);
                                //printf("%s\n", bio_feedback);
                                cJSON * bio_root = cJSON_Parse(bio_feedback);
                                cJSON * bio_type = cJSON_GetObjectItemCaseSensitive(bio_root, "type");
                                char * bio_type_rendered = cJSON_Print(bio_type);
                                remove_doublequot(bio_type_rendered);
                                if (strcmp(bio_type_rendered, "Successful") == 0) {
                                    printf("\nyour bio set successfully.\n");
                                } else {
                                    printf("\nyou can set bio just once\n");
                                }
                            }
                            else if (personal_key == 2) {
                                char cur_password[1000];
                                char new_password[1000];
                                char c_new_password[1000];
                                do {
                                    printf("enter your current password:\n");
                                    scanf("%s", cur_password);
                                    printf("enter your new password:\n");
                                    scanf("%s", new_password);
                                    printf("re-enter your new password:\n");
                                    scanf("%s", c_new_password);

                                } while (confirm_password(new_password, c_new_password) == 0);
                                char * change_password_feedback;
                                char to_send_change_password[1000] = "change password ";
                                strcat(to_send_change_password, token);
                                strcat(to_send_change_password, middle);
                                strcat(to_send_change_password, cur_password);
                                strcat(to_send_change_password, middle);
                                strcat(to_send_change_password, new_password);
                                strcat(to_send_change_password, "\n");
                                change_password_feedback = send_data(to_send_change_password);
                                //printf("%s\n", change_password_feedback);
                                cJSON * change_password_root = cJSON_Parse(change_password_feedback);
                                cJSON * change_password_message = cJSON_GetObjectItemCaseSensitive(change_password_root, "message");
                                char * change_password_message_rendered = cJSON_Print(change_password_message);
                                remove_doublequot(change_password_message_rendered);
                                printf("\n%s\n", change_password_message_rendered);
                            }
                        }
                    }

                    else if (command == 5) {
                        //printf("\n%s\n", token);
                        char * logout_feedback;
                        char to_send_logout[1000] = "logout ";
                        strcat(to_send_logout, token);
                        strcat(to_send_logout, "\n");
                        logout_feedback = send_data(to_send_logout);
                        //printf("%s\n", logout_feedback);
                        cJSON * logout_root = cJSON_Parse(logout_feedback);
                        cJSON * logout_type = cJSON_GetObjectItemCaseSensitive(logout_root, "type");
                        char * logout_type_rendered = cJSON_Print(logout_type);
                        remove_doublequot(logout_type_rendered);
                        if (strcmp(logout_type_rendered, "Successful") == 0) {
                            printf("\nlogout successfully.\nHope to see you again...!\n");
                        }
                        break;

                    }
                }
            } else {
                printf("\nyou are logged in or something went wrong.\n");
            }

        }
        else if (key == 2) {
            char username[1000];
            char password[1000];
            char c_password[1000];
            char * signup_type_rendered;
            do {
                printf("enter your username:\n");
                scanf("%s", username);
                do {
                    printf("enter your password:\n");
                    scanf("%s",password);
                    printf("re-enter your password:\n");
                    scanf("%s",c_password);
                } while (confirm_password(password, c_password) == 0);
                //printf("\n%s\n%s\n", username,password);
                char * signup_feedback;
                char to_send_signup[1000] = "signup ";
                strcat(to_send_signup, username);
                strcat(to_send_signup, middle);
                strcat(to_send_signup, password);
                strcat(to_send_signup, "\n");
                signup_feedback = send_data(to_send_signup);
                //printf("%s\n", signup_feedback);
                cJSON * signup_root = cJSON_Parse(signup_feedback);
                cJSON * signup_type = cJSON_GetObjectItemCaseSensitive(signup_root, "type");
                signup_type_rendered = cJSON_Print(signup_type);
                remove_doublequot(signup_type_rendered);
                //printf("\n%s\n", type_rendered);
                if (strcmp(signup_type_rendered, "Error") == 0) {
                    printf("this username is already exists!\nplease try again to signup.\n");
                }
            } while (strcmp(signup_type_rendered, "Error") == 0);
            if (strcmp(signup_type_rendered, "Successful") == 0) {
                printf("\nyou signup successfully.\n");
                printf("\nhi %s :)\nwelcome to the worst social media ever!\n", username);
                printf("\nnow login to your account and enjoy.\n");
            }

        }
        else if (key == 3) {
            break;
        }
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
    closesocket(client_socket);
    return buffer;
}

void print_register_menu (){
    printf("\n1.log in\n2.sign up\n3.exit\nplease enter the number of command :\n");
}

int confirm_password ( char password[], char c_password[] ){
    if ( strcmp( password, c_password )==0 ){
        printf("your password and re-entered password is equal.\n");
        return 1;
    } else {
        printf("your password and re-entered password is not equal!\nplease enter again.\n");
        return 0;
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

void print_main_menu () {
    printf("\n1.Timeline\n2.Search\n3.Tweet profile\n4.Personal area\n5.Log out\n6.exit\nplease enter the number of command :\n");
}

void print_timeline () {
    printf("\n1.send tweet\n2.refresh\n3.back\nplease enter the number of command :\n");
}

void print_Personal() {
    printf("\n1.set bio\n2.change password\n3.back\nplease enter the number of command :\n");
}

