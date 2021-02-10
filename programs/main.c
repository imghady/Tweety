#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include "cJSON.h"
#define PORT 12345


int initialize(int);
char *send_data(char *);

void print_register_menu ();
void print_main_menu ();
void print_timeline ();
void print_Personal ();
void print_follow ();
void print_unfollow ();
void print_like ();
int confirm_password ( char password[], char c_password[] );
void remove_doublequot (char * input);

struct sockaddr_in server;

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
                                char * refresh_feedback;
                                char to_send_refresh[1000] = "refresh ";
                                strcat(to_send_refresh, token);
                                strcat(to_send_refresh, "\n");
                                refresh_feedback = send_data(to_send_refresh);
                                //printf("%s\n", refresh_feedback);

                                cJSON * root_refresh = cJSON_Parse(refresh_feedback);
                                cJSON * refresh_message = cJSON_GetObjectItemCaseSensitive(root_refresh, "message");
                                //char * refresh_message_rendered = cJSON_Print(refresh_message);
                                //printf("\n%s\n", refresh_message_rendered);
                                int number_of_tweets_search = cJSON_GetArraySize(refresh_message);

                                printf("-------------------------------------------");
                                for (int x = 0; x < number_of_tweets_search; x++) {
                                    cJSON * each_tweet_refresh = cJSON_GetArrayItem(refresh_message, x);
                                    char * each_tweet_rendered_refresh = cJSON_Print(each_tweet_refresh);
                                    //printf("\ntweet %d is: %s\n", x, each_tweet_rendered_refresh);

                                    cJSON * root_tweets_id_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                    cJSON * refresh_tweets_id = cJSON_GetObjectItemCaseSensitive(root_tweets_id_refresh, "id");
                                    char * refresh_tweets_id_rendered = cJSON_Print(refresh_tweets_id);

                                    cJSON * root_tweets_author_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                    cJSON * refresh_tweets_author = cJSON_GetObjectItemCaseSensitive(root_tweets_author_refresh, "author");
                                    char * refresh_tweets_author_rendered = cJSON_Print(refresh_tweets_author);
                                    remove_doublequot(refresh_tweets_author_rendered);

                                    cJSON * root_tweets_content_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                    cJSON * refresh_tweets_content = cJSON_GetObjectItemCaseSensitive(root_tweets_content_refresh, "content");
                                    char * refresh_tweets_content_rendered = cJSON_Print(refresh_tweets_content);
                                    remove_doublequot(refresh_tweets_content_rendered);

                                    cJSON * root_tweets_comments_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                    cJSON * refresh_tweets_comments = cJSON_GetObjectItemCaseSensitive(root_tweets_comments_refresh, "comments");
                                    //char * refresh_tweets_comments_rendered = cJSON_Print(refresh_tweets_comments);

                                    int number_of_comments_refresh = cJSON_GetArraySize(refresh_tweets_comments);

                                    cJSON * root_tweets_likes_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                    cJSON * refresh_tweets_likes = cJSON_GetObjectItemCaseSensitive(root_tweets_likes_refresh,"likes");
                                    char * refresh_tweets_likes_rendered = cJSON_Print(refresh_tweets_likes);

                                    printf("\ntweet id: %s\n", refresh_tweets_id_rendered);
                                    printf("\nauthor: %s\n", refresh_tweets_author_rendered);
                                    printf("\ntweet: %s\n", refresh_tweets_content_rendered);
                                    printf("\nlikes: %s\n", refresh_tweets_likes_rendered);
                                    printf("\nnumber of comments: %d\n", number_of_comments_refresh);
                                    printf("-------------------------------------------");
                                }

                                print_like();
                                int like_key;
                                scanf("%d", &like_key);
                                if (like_key == 4) {

                                }
                                else if (like_key == 1) {
                                    printf("\nplease enter the tweet id:\n");
                                    char tweet_id_to_like[100];
                                    scanf("%s", tweet_id_to_like);
                                    char * like_feedback;
                                    char to_send_like[1000] = "like ";
                                    strcat(to_send_like, token);
                                    strcat(to_send_like, middle);
                                    strcat(to_send_like, tweet_id_to_like);
                                    strcat(to_send_like, "\n");
                                    like_feedback = send_data(to_send_like);
                                    //printf("%s\n", like_feedback);
                                    cJSON * root_like = cJSON_Parse(like_feedback);
                                    cJSON * like_message = cJSON_GetObjectItemCaseSensitive(root_like, "message");
                                    char * like_message_rendered = cJSON_Print(like_message);
                                    remove_doublequot(like_message_rendered);
                                    printf("\n%s\n", like_message_rendered);
                                }
                                else if (like_key == 2) {
                                    printf("\nplease enter the tweet id:\n");
                                    char tweet_id_to_comment[100];
                                    scanf("%s", tweet_id_to_comment);
                                    char comment[1000];
                                    printf("\nwrite your comment:\n");
                                    scanf("%*c%[^\n]s", comment);
                                    char * comment_feedback;
                                    char to_send_comment[1000] = "comment ";
                                    strcat(to_send_comment, token);
                                    strcat(to_send_comment, middle);
                                    strcat(to_send_comment, tweet_id_to_comment);
                                    strcat(to_send_comment, middle);
                                    strcat(to_send_comment, comment);
                                    strcat(to_send_comment, "\n");
                                    comment_feedback = send_data(to_send_comment);
                                    //printf("%s\n", comment_feedback);
                                    cJSON * root_comment = cJSON_Parse(comment_feedback);
                                    cJSON * comment_message = cJSON_GetObjectItemCaseSensitive(root_comment, "message");
                                    char * comment_message_rendered = cJSON_Print(comment_message);
                                    remove_doublequot(comment_message_rendered);
                                    printf("\n%s\n", comment_message_rendered);
                                }
                                else if (like_key == 3) {
                                    printf("\nplease enter the tweet id:\n");
                                    char tweet_id_to_show_comments[100];
                                    scanf("%s", tweet_id_to_show_comments);
                                    printf("-------------------------------------------");
                                    for (int z = 0; z < number_of_tweets_search; z++) {
                                        cJSON * each_tweet_refresh = cJSON_GetArrayItem(refresh_message, z);
                                        char * each_tweet_rendered_refresh = cJSON_Print(each_tweet_refresh);
                                        //printf("\ntweet %d is: %s\n", z, each_tweet_rendered_refresh);

                                        cJSON * root_tweets_id_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                        cJSON * refresh_tweets_id = cJSON_GetObjectItemCaseSensitive(root_tweets_id_refresh, "id");
                                        char * refresh_tweets_id_rendered = cJSON_Print(refresh_tweets_id);

                                        cJSON * root_tweets_author_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                        cJSON * refresh_tweets_author = cJSON_GetObjectItemCaseSensitive(root_tweets_author_refresh, "author");
                                        char * refresh_tweets_author_rendered = cJSON_Print(refresh_tweets_author);
                                        remove_doublequot(refresh_tweets_author_rendered);

                                        cJSON * root_tweets_content_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                        cJSON * refresh_tweets_content = cJSON_GetObjectItemCaseSensitive(root_tweets_content_refresh, "content");
                                        char * refresh_tweets_content_rendered = cJSON_Print(refresh_tweets_content);
                                        remove_doublequot(refresh_tweets_content_rendered);

                                        cJSON * root_tweets_comments_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                        cJSON * refresh_tweets_comments = cJSON_GetObjectItemCaseSensitive(root_tweets_comments_refresh, "comments");
                                        //char * refresh_tweets_comments_rendered = cJSON_Print(refresh_tweets_comments);

                                        int number_of_comments_refresh = cJSON_GetArraySize(refresh_tweets_comments);

                                        cJSON * root_tweets_likes_refresh = cJSON_Parse(each_tweet_rendered_refresh);
                                        cJSON * refresh_tweets_likes = cJSON_GetObjectItemCaseSensitive(root_tweets_likes_refresh,"likes");
                                        char * refresh_tweets_likes_rendered = cJSON_Print(refresh_tweets_likes);

                                        if (strcmp(tweet_id_to_show_comments,refresh_tweets_id_rendered) == 0) {
                                            printf("\ntweet id: %s\n", refresh_tweets_id_rendered);
                                            printf("\nauthor: %s\n", refresh_tweets_author_rendered);
                                            printf("\ntweet: %s\n", refresh_tweets_content_rendered);
                                            printf("\nlikes: %s\n", refresh_tweets_likes_rendered);
                                            printf("\nnumber of comments: %d\n", number_of_comments_refresh);

                                            printf("- - - - - - - - - - - - - - - - - - - - - -\n");
                                            for (int a = 0; a < number_of_comments_refresh; a++) {
                                                cJSON * each_comment = cJSON_GetArrayItem(refresh_tweets_comments, a);
                                                char * each_comment_rendered = cJSON_Print(each_comment);
                                                remove_doublequot(each_comment_rendered);
                                                //char  * each_comment_rendered_author = each_comment->string;
                                                printf("\nauthor of comment %d is: %s\n", a + 1, each_comment->string);
                                                printf("\ncomment %d is: %s\n", a + 1, each_comment_rendered);
                                                printf("- - - - - - - - - - - - - - - - - - - - - -\n");
                                            }
                                            printf("-------------------------------------------");
                                        }
                                    }
                                }
                                else {
                                    printf("\ninvalid command\n");
                                }

                            }


                        }
                    }

                    else if (command == 2) {
                        printf("\nenter the username that you want to search for:\n");
                        char search_username[1000];
                        scanf("%s", search_username);
                        char * search_feedback;
                        char to_send_search[1000] = "search ";
                        strcat(to_send_search, token);
                        strcat(to_send_search, middle);
                        strcat(to_send_search, search_username);
                        strcat(to_send_search, "\n");
                        search_feedback = send_data(to_send_search);
                        printf("%s\n", search_feedback);

                        cJSON * root_search = cJSON_Parse(search_feedback);
                        cJSON * search_type = cJSON_GetObjectItemCaseSensitive(root_search, "type");
                        char * search_type_rendered = cJSON_Print(search_type);
                        remove_doublequot(search_type_rendered);
                        //printf("\ntype: %s\n", search_type_rendered);
                        if (strcmp(search_type_rendered, "Error") == 0) {
                            //printf("\nerror\n");
                            cJSON *search_message = cJSON_GetObjectItemCaseSensitive(root_search, "message");
                            char *search_message_rendered = cJSON_Print(search_message);
                            remove_doublequot(search_message_rendered);
                            printf("\n%s\n", search_message_rendered);
                        } else {

                            cJSON *search_message = cJSON_GetObjectItemCaseSensitive(root_search, "message");
                            char *search_message_rendered = cJSON_Print(search_message);
                            //printf("\n%s\n", search_message_rendered);

                            cJSON *root_username_search = cJSON_Parse(search_message_rendered);
                            cJSON *search_username_in = cJSON_GetObjectItemCaseSensitive(root_username_search,"username");
                            char *search_username_rendered = cJSON_Print(search_username_in);
                            remove_doublequot(search_username_rendered);

                            cJSON *root_bio_search = cJSON_Parse(search_message_rendered);
                            cJSON *search_bio = cJSON_GetObjectItemCaseSensitive(root_bio_search, "bio");
                            char *search_bio_rendered = cJSON_Print(search_bio);
                            remove_doublequot(search_bio_rendered);

                            cJSON *root_followers_search = cJSON_Parse(search_message_rendered);
                            cJSON *search_followers = cJSON_GetObjectItemCaseSensitive(root_followers_search,"numberOfFollowers");
                            char *search_followers_rendered = cJSON_Print(search_followers);

                            cJSON *root_followings_search = cJSON_Parse(search_message_rendered);
                            cJSON *search_followings = cJSON_GetObjectItemCaseSensitive(root_followings_search,"numberOfFollowings");
                            char *search_followings_rendered = cJSON_Print(search_followings);

                            cJSON *root_status_search = cJSON_Parse(search_message_rendered);
                            cJSON *search_status = cJSON_GetObjectItemCaseSensitive(root_status_search, "followStatus");
                            char *search_status_rendered = cJSON_Print(search_status);
                            remove_doublequot(search_status_rendered);

                            cJSON *root_tweets_search = cJSON_Parse(search_message_rendered);
                            cJSON *search_tweets = cJSON_GetObjectItemCaseSensitive(root_tweets_search, "allTweets");
                            //char * search_tweets_rendered = cJSON_Print(search_tweets);
                            int number_of_tweets_search = cJSON_GetArraySize(search_tweets);


                            printf("\nusername: %s\n", search_username_rendered);
                            printf("\nbio: %s\n", search_bio_rendered);
                            printf("\nnumber of followers: %s\n", search_followers_rendered);
                            printf("\nnumber of followings: %s\n", search_followings_rendered);
                            printf("\nfollow status: %s\n", search_status_rendered);
                            //printf("\nall tweets: %s\n", search_tweets_rendered);
                            printf("\nnumber of tweets: %d\n", number_of_tweets_search);
                            printf("-------------------------------------------");

                            for (int w = 0; w < number_of_tweets_search; w++) {
                                cJSON *each_tweet_search = cJSON_GetArrayItem(search_tweets, w);
                                char *each_tweet_rendered_search = cJSON_Print(each_tweet_search);
                                //printf("\ntweet %d is: %s\n", k, each_tweet_rendered_search);

                                cJSON *root_tweets_id_search = cJSON_Parse(each_tweet_rendered_search);
                                cJSON *search_tweets_id = cJSON_GetObjectItemCaseSensitive(root_tweets_id_search, "id");
                                char *search_tweets_id_rendered = cJSON_Print(search_tweets_id);

                                cJSON *root_tweets_author_search = cJSON_Parse(each_tweet_rendered_search);
                                cJSON *search_tweets_author = cJSON_GetObjectItemCaseSensitive(root_tweets_author_search, "author");
                                char *search_tweets_author_rendered = cJSON_Print(search_tweets_author);
                                remove_doublequot(search_tweets_author_rendered);

                                cJSON *root_tweets_content_search = cJSON_Parse(each_tweet_rendered_search);
                                cJSON *search_tweets_content = cJSON_GetObjectItemCaseSensitive(root_tweets_content_search, "content");
                                char *search_tweets_content_rendered = cJSON_Print(search_tweets_content);
                                remove_doublequot(search_tweets_content_rendered);

                                cJSON * root_tweets_comments_search = cJSON_Parse(each_tweet_rendered_search);
                                cJSON * search_tweets_comments = cJSON_GetObjectItemCaseSensitive(root_tweets_comments_search, "comments");
                                //char * search_tweets_comments_rendered = cJSON_Print(search_tweets_comments);

                                int number_of_comments_search = cJSON_GetArraySize(search_tweets_comments);

                                cJSON *root_tweets_likes_search = cJSON_Parse(each_tweet_rendered_search);
                                cJSON *search_tweets_likes = cJSON_GetObjectItemCaseSensitive(root_tweets_likes_search,"likes");
                                char *search_tweets_likes_rendered = cJSON_Print(search_tweets_likes);

                                printf("\ntweet id: %s\n", search_tweets_id_rendered);
                                printf("\nauthor: %s\n", search_tweets_author_rendered);
                                printf("\ntweet: %s\n", search_tweets_content_rendered);
                                printf("\nlikes: %s\n", search_tweets_likes_rendered);
                                printf("\nnumber of comments: %d\n", number_of_comments_search);
                                printf("-------------------------------------------");
                            }
                            if (strcmp(search_status_rendered, "NotFollowed") == 0) {
                                print_follow();
                                int follow_key;
                                scanf("%d",&follow_key);
                                if (follow_key == 1) {
                                    char * follow_feedback;
                                    char to_send_follow[1000] = "follow ";
                                    strcat(to_send_follow, token);
                                    strcat(to_send_follow, middle);
                                    strcat(to_send_follow, search_username);
                                    strcat(to_send_follow, "\n");
                                    follow_feedback = send_data(to_send_follow);
                                    //printf("%s\n", follow_feedback);
                                    cJSON * follow_search = cJSON_Parse(follow_feedback);
                                    cJSON * follow_type = cJSON_GetObjectItemCaseSensitive(follow_search, "type");
                                    char * follow_type_rendered = cJSON_Print(follow_type);
                                    remove_doublequot(follow_type_rendered);
                                    //printf("\n%s\n", follow_type_rendered);
                                    if (strcmp(follow_type_rendered,"Successful") == 0) {
                                        printf("\nyou successfully follow %s\nnow you can share your tweets together ;)\n",search_username);
                                    } else {
                                        printf("\nsomething went wrong\n");
                                    }
                                }

                            } else {
                                print_unfollow();
                                int unfollow_key;
                                scanf("%d",&unfollow_key);
                                if (unfollow_key == 1) {
                                    char * unfollow_feedback;
                                    char to_send_unfollow[1000] = "unfollow ";
                                    strcat(to_send_unfollow, token);
                                    strcat(to_send_unfollow, middle);
                                    strcat(to_send_unfollow, search_username);
                                    strcat(to_send_unfollow, "\n");
                                    unfollow_feedback = send_data(to_send_unfollow);
                                    //printf("%s\n", unfollow_feedback);
                                    cJSON * unfollow_search = cJSON_Parse(unfollow_feedback);
                                    cJSON * unfollow_type = cJSON_GetObjectItemCaseSensitive(unfollow_search, "type");
                                    char * unfollow_type_rendered = cJSON_Print(unfollow_type);
                                    remove_doublequot(unfollow_type_rendered);
                                    //printf("\n%s\n", unfollow_type_rendered);
                                    if (strcmp(unfollow_type_rendered,"Successful") == 0) {
                                        printf("\nyou successfully unfollow %s\ngod bless him...\n",search_username);
                                    } else {
                                        printf("\nsomething went wrong\n");
                                    }
                                }
                            }
                        }

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
                        //char * profile_tweets_rendered = cJSON_Print(profile_tweets);
                        int number_of_tweets = cJSON_GetArraySize(profile_tweets);


                        printf("\nyour username: %s\n", profile_username_rendered);
                        printf("\nyour bio: %s\n", profile_bio_rendered);
                        printf("\nnumber of your followers: %s\n", profile_followers_rendered);
                        printf("\nnumber of your followings: %s\n", profile_followings_rendered);
                        //printf("\nall tweets: %s\n", profile_tweets_rendered);
                        printf("\nnumber of your tweets: %d\n", number_of_tweets);
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

                            cJSON * root_tweets_comments_profile = cJSON_Parse(each_tweet_rendered);
                            cJSON * profile_tweets_comments = cJSON_GetObjectItemCaseSensitive(root_tweets_comments_profile, "comments");
                            //char * profile_tweets_comments_rendered = cJSON_Print(profile_tweets_comments);

                            int number_of_comments = cJSON_GetArraySize(profile_tweets_comments);
                            //printf("\nn o c: %d", number_of_comments);


                            cJSON * root_tweets_likes_profile = cJSON_Parse(each_tweet_rendered);
                            cJSON * profile_tweets_likes = cJSON_GetObjectItemCaseSensitive(root_tweets_likes_profile, "likes");
                            char * profile_tweets_likes_rendered = cJSON_Print(profile_tweets_likes);

                            printf("\ntweet id: %s\n", profile_tweets_id_rendered);
                            printf("\nauthor: %s\n", profile_tweets_author_rendered);
                            printf("\ntweet: %s\n", profile_tweets_content_rendered);
                            //printf("\ncomments: %s\n", profile_tweets_comments_rendered);
                            printf("\nlikes: %s\n", profile_tweets_likes_rendered);
                            printf("\nnumber of comments: %d\n", number_of_comments);

                            printf("- - - - - - - - - - - - - - - - - - - - - -\n");
                            for (int y = 0; y < number_of_comments; y++) {
                                cJSON *each_comment = cJSON_GetArrayItem(profile_tweets_comments, y);
                                char * each_comment_rendered = cJSON_Print(each_comment);
                                remove_doublequot(each_comment_rendered);
                                //char  * each_comment_rendered_author = each_comment->string;
                                printf("\nauthor of comment %d is: %s\n", y + 1, each_comment->string);
                                printf("\ncomment %d is: %s\n", y + 1, each_comment_rendered);
                                printf("- - - - - - - - - - - - - - - - - - - - - -\n");
                            }
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

                    } else {
                        printf("\ninvalid command\n");
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
        else {
            printf("\ninvalid command\n");
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

void print_Personal () {
    printf("\n1.set bio\n2.change password\n3.back\nplease enter the number of command :\n");
}

void print_follow () {
    printf("\n1.follow this user\n2.back\nplease enter the number of command :\n");
}

void print_unfollow () {
    printf("\n1.unfollow this user\n2.back\nplease enter the number of command :\n");
}

void print_like () {
    printf("\n1.like a tweet\n2.comment for a tweet\n3.show comments\n4.back\nplease enter the number of command :\n");
}


