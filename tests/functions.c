#include <stdio.h>
#include <string.h>

int main (){
    //print_register_menu ();
    //print_main_menu ();
    //get_string();
    /*char string1[1000], string2[1000];
    scanf("%s%s", string1 ,string2 );
    remove_doublequot(string1);
    remove_doublequot(string2);
    printf("%s\n%s\n", string1, string2 );*/
    char string1[1000];
    scanf("%[^\n]s", string1);
    printf("%s", string1);
}

void print_register_menu (){
    printf("\n1.log in\n2.sign up\n3.exit\n");
}

void print_main_menu (){
    printf("\n1.Timeline\n2.Search\n3.Tweet profile\n4.Personal area\n5.Log out\n6.exit\n");
}

int confirm_password ( char password[], char c_password[] ){
    if ( strcmp( password, c_password )==0 ){
        printf("your password set successfully");
        return 1;
    } else {
        printf("your password and re-entered password is not equal!\nplease enter again.");
        return 0;
    }
}

int get_string(){
    char string1[1000], string2[1000];
    scanf("%s%s", string1 ,string2 );
    printf("%s\n%s\n", string1, string2 );
    printf("%d", strcmp( string1 , string2));
}

int get_password(){
    char password[1000], c_password[1000];
    printf("enter your password\n");
    gets(password);
    printf("re-enter your password\n");
    gets(c_password);
}

void remove_doublequot (char * input) {
    int i = 0;
    while (*(input + i) != NULL){
        *(input + i) = *(input + i + 1);
        i++;
    }
    *(input + i - 2) = '\0';
}
