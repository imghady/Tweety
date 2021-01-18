



    int num;
        scanf("%d", &num);
        if (num == 1){
            char username[100];
            char password[100];
            char * feedback;
            printf("username :\n");
            scanf("%s",username);
            printf("password :\n");
            scanf("%s",password);
            feedback = send_data("signup <username>, <password>\n");
            printf("%s\n", feedback);
        }else{

        }
