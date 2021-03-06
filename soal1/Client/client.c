#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h> 
#include <netdb.h> 
#include <string.h>
#include <ctype.h>

#define BUFFER_SIZE 1000
#define MAX 50000
#define SERVER_FREE "1"
#define SERVER_OCCUPIED "2"
#define LOGIN "1"
#define REGISTER "2"
#define LOGIN_FAILED "101"
#define LOGIN_SUCCESS "102"
#define ADD "add"
#define SEE "see"
#define DOWNLOAD "download"
#define DELETE "delete"
#define FIND "find"
#define FOUND "202"
#define SUCCESS "303"
#define ERROR "404"

// Client Variable
struct sockaddr_in saddr;
int fd, ret_val;
struct hostent *local_host; /* need netdb.h for this */
char message[BUFFER_SIZE];

// Client function
void send_id_pass();
void send_file_data();
void see_files();
void find_files();
void download_file(char filename[]);
void delete_file();

int main () {

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", hstrerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Let us initialize the server address structure */
    saddr.sin_family = AF_INET;         
    saddr.sin_port = htons(7000);     
    local_host = gethostbyname("127.0.0.1");
    saddr.sin_addr = *((struct in_addr *)local_host->h_addr);

    /* Step2: connect to the TCP server socket */
    ret_val = connect(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val == -1) {
        fprintf(stderr, "connect failed [%s]\n", hstrerror(errno));
        close(fd);
        return -1;
    }

    // Check if server is occupied or not
    printf("Pending...\n");
    ret_val = recv(fd, message, sizeof(message), 0);
    while(strcmp(message, SERVER_OCCUPIED) == 0){
        ret_val = recv(fd, message, sizeof(message), 0);
    }
    
    // Client Variables
    bool login;

    // Inititate first state for client
    login = false;

    // Welcome Message
    printf("\nWelcome to nothing1\n");
    

    while (1)
    {
        // Login and Register
        while(!login){
            printf("1. Login\n2. Register\n(input number)\n");
            scanf("%s",message);
            ret_val = send(fd, message, sizeof(message),0);
            if(strcmp(message, LOGIN)==0){
                ret_val = recv(fd, message, sizeof(message), 0);
                printf("%s\n", message);
                send_id_pass();
                ret_val = recv(fd,message, sizeof(message), 0);
                if(strcmp(message, LOGIN_SUCCESS) == 0){
                    printf("\nYou are logged in!\n");
                    login = true;
                }
                else{
                    printf("\nWrong id or password!\n");
                    login = false;
                }
            }
            else if(strcmp(message, REGISTER)==0){
                ret_val = recv(fd, message, sizeof(message), 0);
                printf("%s\n", message);
                send_id_pass();
                ret_val = recv(fd, message, sizeof(message), 0);
                printf("\n%s\n", message);
                login = true;
            }
            else{
                printf("Wrong input!\n");
            }
        }

        // App
        while(1){
            printf("Masukkan Command: ");
            char arg1[BUFFER_SIZE/3], arg2[BUFFER_SIZE/3];
            scanf("%s", arg1);
            strcpy(message, arg1);
            ret_val = send(fd, message, sizeof(message), 0);
            if(strcmp(arg1, ADD) == 0){
                ret_val = recv(fd, message, sizeof(message), 0);
                printf("%s\n", message);
                send_file_data();
                ret_val = recv(fd, message, sizeof(message), 0);
                printf("%s\n", message);
            }
            else if(strcmp(arg1, SEE)==0){
                ret_val = recv(fd, message, sizeof(message), 0);
                printf("%s\n", message);
                see_files();
            }
            else if(strcmp(arg1, FIND)==0){
                scanf("%s", arg2);
                strcpy(message, arg2);
                ret_val = send(fd, message, sizeof(message), 0);
                find_files();
            }
            else if(strcmp(arg1, DOWNLOAD)==0){
                scanf("%s", arg2);
                strcpy(message, arg2);
                ret_val = send(fd, message, sizeof(message), 0);
                download_file(arg2);
            }
            else if(strcmp(arg1, DELETE)==0){
                scanf("%s", arg2);
                strcpy(message, arg2);
                ret_val = send(fd, message, sizeof(message), 0);
                delete_file();
            }
        }
    }

    /* Last step: close the socket */
    close(fd);
    return 0;
}

void send_id_pass(){
    char id[100], pass[100];
    printf("ID: ");
    scanf("%s", id);
    printf("Password: ");
    scanf("%s", pass);
    sprintf(message, "%s:%s", id, pass);
    ret_val = send(fd, message, sizeof(message), 0);
    return;
}

void send_file_data(){
    // Publisher
    printf("Publisher: ");
    scanf("%s", message);
    ret_val = send(fd, message, sizeof(message), 0);
    // Tahun
    printf("Tahun Publikasi: ");
    scanf("%s", message);
    ret_val = send(fd, message, sizeof(message), 0);
    // Filepath
    printf("Filepath: ");
    scanf("%s", message);
    ret_val = send(fd, message, sizeof(message), 0);

    // Read file content
    FILE *fp = fopen(message, "rb"); // Last message is the filepath
    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *string = malloc(fsize+1);
    fread(string, 1, fsize, fp);
    fclose(fp);
    // Send file content
    strcpy(message, string);
    ret_val = send(fd, message, sizeof(message), 0);
}

void see_files(){
    char deskripsi_total[MAX];
    ret_val = recv(fd, deskripsi_total, sizeof(deskripsi_total), 0);
    printf("%s\n", deskripsi_total);
}

void find_files(){
    char files_list[MAX];
    ret_val = recv(fd, files_list, sizeof(files_list), 0);
    printf("\nFile yang ditemukan:\n%s\n", files_list);
}

void download_file(char filename[]){
    ret_val = recv(fd, message, sizeof(message), 0);
    // Check if file exist
    if(strcmp(message, ERROR)==0){
        printf("\nError: File not found\n");
    }
    else{
        printf("\nFile found, Downloading...\n");
        ret_val = recv(fd, message, sizeof(message), 0);
        FILE *fp = fopen(filename, "w+");
        fputs(message, fp);
        fclose(fp);
        printf("Success Download\n\n");
    }
}

void delete_file(){
    ret_val = recv(fd, message, sizeof(message), 0);
    // Check if success
    if(strcmp(message, ERROR)==0){
        printf("\nError: File not found\n");
    }
    else{
        printf("\nFile successfully deleted\n");
    }
}