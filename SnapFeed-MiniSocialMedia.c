#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#define MAX_USERNAME 20
#define MAX_PASSWORD 20
#define MAX_FEED 280

char loggedInUser[MAX_USERNAME] = ""; // Global string to store logged-in username and "" is to initialise as empty

void registerUser();
int userLogIn(char *loggedInUser);
void menu(const char *username);
void addPost(const char *username);
void viewPosts();

int main() {
    int choice;
    char loggedInUser[50]; // To store the username of the logged-in user

    do {
        printf("\nWelcome to SnapFeed-Mini Social Media Feed!\n");
        printf("1. Register\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                registerUser(); // Call registration function
                break;

            case 2:
                if (userLogIn(loggedInUser)) { // If login is successful
                    // printf("Welcome, %s!\n", loggedInUser);
                    menu(loggedInUser); // Navigate to the user menu
                }
                break;

            case 3:
                printf("Thank you for using SnapFeed-Mini Social Media Feed! Goodbye.\n");
                break;

            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 3);

    return 0;
}

void registerUser(){
    char username[MAX_USERNAME], password[MAX_PASSWORD], securityAnswer[100];
    int securityQuestion;

    FILE* fptr;
    FILE* ptr;
    fptr = fopen("user.txt", "a");
    ptr = fopen("savedCredentials.txt", "w");
    if(fptr == NULL || ptr == NULL){
        printf("Error: Could not open user's file :( ...\n");
        return;
    }
    printf("Enter username : ");
    scanf("%s", &username);
    printf("Enter password : ");
    scanf("%s", &password);

    printf("Select a security question:\n");
    printf("1. What is your pet's name?\n");
    printf("2. What is your mother's maiden name?\n");
    printf("3. What is your favorite color?\n");
    printf("Enter the number corresponding to your question: ");
    scanf("%d", &securityQuestion);
    getchar(); // Clear input buffer
    printf("Enter your answer: ");
    fgets(securityAnswer, 100, stdin);
    securityAnswer[strcspn(securityAnswer, "\n")] = 0; // Remove trailing newline

    fprintf(fptr, "%s %s %d %s\n", username, password, securityQuestion, securityAnswer);
    printf("Do you want to save your unsername and password for auto login? (yes/no) : ");
    char choice[4];
    scanf("%s", choice);
    if(strcmp(choice, "yes") == 0){
        fprintf(ptr, "%s %s\n", username, password);
        printf("Credentials saved for auto-login.\n");
    }

    fclose(fptr);
    fclose(ptr);
    printf("Registeration done successfully!!!\n");
}

int userLogIn(char *loggedInUser){
    char username[MAX_USERNAME], password[MAX_PASSWORD], fileUser[MAX_USERNAME], filePass[MAX_PASSWORD];
    char savedUsername[MAX_USERNAME], savedPassword[MAX_PASSWORD], securityAnswer[100], storedAnswer[100];
    int securityQuestion;

    FILE* fptr;
    FILE* ptr;  
    fptr = fopen("user.txt", "r");
    ptr = fopen("savedCredentials.txt", "r");

    if(fptr == NULL){
        printf("Error: Could not open user's file :( ...\n");
        return 0;
    }

    if (ptr) {
        fscanf(ptr, "%s %s", savedUsername, savedPassword);
        fclose(ptr);
        printf("Do you want to use saved credentials? (yes/no): ");
        char choice[4];
        scanf("%s", choice);
        if (strcmp(choice, "yes") == 0) {
            strcpy(username, savedUsername);
            strcpy(password, savedPassword);
        } else {
            printf("Enter your username: ");
            scanf("%s", username);
            printf("Enter your password: ");
            scanf("%s", password);
        }
    } else {
        printf("Enter your username: ");
        scanf("%s", username);
        printf("Enter your password: ");
        scanf("%s", password);
    }

    while (fscanf(fptr, "%s %s\n", fileUser, filePass) != EOF){
        if(strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0){
            strcpy(loggedInUser, username);
            fclose(fptr);
            printf("Log-in done successfully!!!\n");
            printf("WELCOME, %s :)\n", loggedInUser);
            return 1;
        }
    }

    fclose(fptr);
    printf("Invalid username or password!!!\n");

    printf("Forgot password? (yes/no): ");
    char forgetChoice[4];
    scanf("%s", forgetChoice);
    if (strcmp(forgetChoice, "yes") == 0) {
        fptr = fopen("users.txt", "r");
        if (fptr == NULL) {
            printf("Error opening file.\n");
            return 0;
        }

        printf("Enter your username: ");
        scanf("%s", username);

        while (fscanf(fptr, "%s %s %d %[^\n]s", fileUser, filePass, &securityQuestion, storedAnswer) != EOF) {
            if (strcmp(username, fileUser) == 0) {
                printf("Answer the following security question:\n");
                switch (securityQuestion) {
                    case 1: printf("What is your pet's name?\n"); break;
                    case 2: printf("What is your mother's maiden name?\n"); break;
                    case 3: printf("What is your favorite color?\n"); break;
                    default: printf("Unknown question.\n"); break;
                }

                getchar(); // Clear input buffer
                fgets(securityAnswer, 100, stdin);
                securityAnswer[strcspn(securityAnswer, "\n")] = 0; // Remove trailing newline

                if (strcmp(securityAnswer, storedAnswer) == 0) {
                    printf("Answer correct! Enter a new password: ");
                    char newPassword[MAX_PASSWORD];
                    scanf("%s", newPassword);

                    // Update the password in the file
                    FILE *tempFile = fopen("temp.txt", "w");
                    rewind(fptr);
                    while (fscanf(fptr, "%s %s %d %[^\n]s", fileUser, filePass, &securityQuestion, storedAnswer) != EOF) {
                        if (strcmp(username, fileUser) == 0) {
                            fprintf(tempFile, "%s %s %d %s\n", username, newPassword, securityQuestion, storedAnswer);
                        } else {
                            fprintf(tempFile, "%s %s %d %s\n", fileUser, filePass, securityQuestion, storedAnswer);
                        }
                    }
                    fclose(fptr);
                    fclose(tempFile);
                    remove("users.txt");
                    rename("temp.txt", "users.txt");
                    printf("Password reset successfully! You can now log in with your new password.\n");
                    return 0;
                } else {
                    printf("Incorrect answer.\n");
                    fclose(fptr);
                    return 0;
                }
            }
        }
        fclose(fptr);
        printf("Username not found.\n");
    }

    return 0;
}

int generatePostID(){
    int lastID = 0;
    FILE* fptr = fopen("posts.txt", "r");

    if(fptr){
        int postID;
        char username[MAX_USERNAME], timestamp[100], content[MAX_FEED];
        while(fscanf(fptr, "%d %s %s %[^\n]s", &postID, username, timestamp, content) != EOF){
            lastID = postID;
        }
        fclose(fptr);
    }

    return lastID + 1;
}

void addPost(const char *username){
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    char timestamp[100];
    sprintf(timestamp, "%04d-%02d-%02d %02d:%02d:%02d",
            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
            tm.tm_hour, tm.tm_min, tm.tm_sec);

    // if(strlen(loggedInUser) == 0){
    //     printf("Please login first..!\n");
    //     return;
    // }

    char post[MAX_FEED];
    int postID = generatePostID();

    FILE* fptr;
    fptr = fopen("posts.txt", "a");

    if(fptr == NULL){
        printf("Error: Could not open user's file :( ...\n");
        return;
    }

    printf("Enter your post (max %d character) : ", MAX_FEED);
    getchar(); //Clear input buffer.
    fgets(post, MAX_FEED, stdin);
    post[strcspn(post, "\n")] = 0; // Remove trailing newline

    fprintf(fptr, "%d %s %s %s\n", postID, username, timestamp, post);
    fclose(fptr);

    printf("Post added successfully with ID %d!\n", postID);
}

void viewPosts(){
    FILE* fptr;
    fptr = fopen("posts.txt", "r");

    if(fptr == NULL){
        printf("No post available yet.\n");
        return;
    }

    printf("Displaying all the posts : \n");
    printf("--------------------------------------------------\n");

    int postID;
    char username[MAX_USERNAME], timestamp[100], post[MAX_FEED];
    while (fscanf(fptr, "%d %s %s %[^\n]s", &postID, username, timestamp, post) != EOF) {
        printf("Post ID: %d\n", postID);
        printf("Username: %s\n", username);
        printf("Timestamp: %s\n", timestamp);
        printf("Content: %s\n", post);
        printf("--------------------------------------------------\n");
    }

    fclose(fptr);
}

void deletePost(const char *username){
    char line[300];
    FILE* fptr;
    FILE* temp;
    fptr = fopen("posts.txt", "r");
    temp = fopen("tempFile.txt", "w");

    if(fptr == NULL || temp == NULL){
        printf("Error: Could not open files.\n");
        return;
    }

    char keyword[50];
    printf("Enter a keyword from your post to delete: ");
    scanf("%s", keyword);

    int found = 0;
    while (fgets(line, sizeof(line), fptr)) {
        if (strstr(line, loggedInUser) && strstr(line, keyword)) {
            found = 1; // Skip writing this line
            continue;
        }
        fputs(line, temp); // Copy other lines to temp file
    }

    fclose(fptr);
    fclose(temp);

    if (found) {
        remove("posts.txt");
        rename("tempFile.txt", "posts.txt");
        printf("Post deleted successfully.\n");
    } else {
        remove("tempFile.txt");
        printf("No matching post found.\n");
    }
}

void logoutUser() {
    if (strlen(loggedInUser) == 0) {
        printf("No user is currently logged in.\n");
        return;
    }

    printf("User %s logged out successfully.\n", loggedInUser);
    strcpy(loggedInUser, ""); // Clear global variable
}

void menu(const char *username) {
    int choice;

    do {
        printf("\nMenu:\n");
        printf("1. Add a Post\n");
        printf("2. View All Posts\n");
        printf("3. Delete a Post\n");
        printf("4. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addPost(username);
                break;
            case 2:
                viewPosts();
                break;
            case 3:
                deletePost(username);
                break;
            case 4:
                printf("Logging out...\n");
                logoutUser();
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);
}