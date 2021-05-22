#include <stdio.h>
#include <netinet/in.h> 
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h> 
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

#define DATA_BUFFER 1000
#define MAX 50000
#define MAX_CONNECTIONS 10 
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

#define SUCCESS_MESSAGE "Your message delivered successfully"

// Queue Implementation
typedef struct QueueNode{
	int data;
	struct QueueNode *next;
}Qnode;

typedef struct Queue{
	Qnode *front, *rear;
	unsigned size;
}Queue;

void qinit(Queue *q);
bool q_isEmpty(Queue *q);
void qpush(Queue *q, int value);
void qpop(Queue *q);
int qfront(Queue *q);

void qinit(Queue *q){
	q->size = 0;
	q->front = NULL;
	q->rear = NULL;
}

bool q_isEmpty(Queue *q){
	return (q->front == NULL && q->rear == NULL);
}

void qpush(Queue *q, int value){
	Qnode *newNode = (Qnode*) malloc(sizeof(Qnode));
	if(newNode){
		q->size++;
		newNode->data = value;
		newNode->next = NULL;
		if(q_isEmpty(q)){
			q->front = newNode;
			q->rear = newNode;
		}
		else{
			q->rear->next = newNode;
			q->rear = newNode;
		}
	}
}

void qpop(Queue *q){
	if(!q_isEmpty(q)){
		Qnode *temp = q->front;
		q->front = q->front->next;
		free(temp);

		if(q->front == NULL){
			q->rear = NULL;
		}
		q->size--;
	}
}

int qfront(Queue *q){
	if(!q_isEmpty(q)){
		return (q->front->data);
	}
	return (int)0;
}
// End of Queue Implementation


// Server Variables
fd_set read_fd_set;
struct sockaddr_in new_addr;
int server_fd, new_fd, ret_val, i, fd_now;
socklen_t addrlen;
char buf[DATA_BUFFER];
int all_connections[MAX_CONNECTIONS];

int create_tcp_server_socket() {
    struct sockaddr_in saddr;
    int fd, ret_val;

    /* Step1: create a TCP socket */
    fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (fd == -1) {
        fprintf(stderr, "socket failed [%s]\n", strerror(errno));
        return -1;
    }
    printf("Created a socket with fd: %d\n", fd);

    /* Initialize the socket address structure */
    saddr.sin_family = AF_INET;         
    saddr.sin_port = htons(7000);     
    saddr.sin_addr.s_addr = INADDR_ANY; 

    /* Step2: bind the socket to port 7000 on the local host */
    ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
    if (ret_val != 0) {
        fprintf(stderr, "bind failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }

    /* Step3: listen for incoming connections */
    ret_val = listen(fd, 5);
    if (ret_val != 0) {
        fprintf(stderr, "listen failed [%s]\n", strerror(errno));
        close(fd);
        return -1;
    }
    return fd;
}

// Other variables
bool client_login;
char id_pass_now[DATA_BUFFER];
char server_path[DATA_BUFFER];

// Server function
void init_server();
void fill_buff(char *message);
void login(int fd_number);
void regist(int fd_number);
bool check_akun(char id_pass[]);
void save_akun(char id_pass[]);
void add_file(int fd_number);
void take_file_name(char filename[], char file[]);
void save_to_tsv(char publisher[], char tahun[], char filepath[]);
void send_file_to_server(char filepath[], char filecontent[]);
void see(int fd_number);
void find_file(int fd_number);
void download_file(int fd_number);
void delete_file(int fd_number);
void log_activity(char *status, char filename[]);


int main () {

    /* Get the socket server fd */
    server_fd = create_tcp_server_socket(); 
    if (server_fd == -1) {
        fprintf(stderr, "Failed to create a server\n");
        return -1; 
    }   

    /* Initialize all_connections and set the first entry to server fd */
    for (i=0;i < MAX_CONNECTIONS;i++) {
        all_connections[i] = -1;
    }
    all_connections[0] = server_fd;

    // Initial Server
    Queue svQueue;
    qinit(&svQueue);
    fd_now = -1;
    client_login = false;
    init_server();

    printf("\nServer is walking\n");


    while (1) {
        FD_ZERO(&read_fd_set);
        /* Set the fd_set before passing it to the select call */
        for (i=0;i < MAX_CONNECTIONS;i++) {
            if (all_connections[i] >= 0) {
                FD_SET(all_connections[i], &read_fd_set);
            }
        }

        /* Invoke select() and then wait! */
        printf("\nWaiting for event from client(s)\n");
        ret_val = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL);

        /* select() woke up. Identify the fd that has events */
        if (ret_val >= 0 ) {
            printf("Something happened, status: %d\n", ret_val);
            /* Check if the fd with event is the server fd */
            if (FD_ISSET(server_fd, &read_fd_set)) { 
                // printf("Returned fd is %d (server's fd)\n", server_fd);
                /* accept the new connection */
                new_fd = accept(server_fd, (struct sockaddr*)&new_addr, &addrlen);
                if (new_fd >= 0) {
                    for (i=0;i < MAX_CONNECTIONS;i++) {
                        if (all_connections[i] < 0) {
                        	printf("Someone came in with fd: %d\n", new_fd);
                            all_connections[i] = new_fd;
                            qpush(&svQueue, i);
                            if(fd_now == -1 || q_isEmpty(&svQueue)){
                            	fd_now = qfront(&svQueue);
                            	ret_val = send(all_connections[i], SERVER_FREE, sizeof(SERVER_FREE), 0);                            	
                            }
                            else if(i != fd_now){
                            	ret_val = send(all_connections[i], SERVER_OCCUPIED, sizeof(SERVER_OCCUPIED), 0);
                            }                
                            break;
                        }
                    }
                } else {
                    fprintf(stderr, "accept failed [%s]\n", strerror(errno));
                }
                ret_val--;
                if (!ret_val) continue;
            } 

            /* Check if the fd with event is a non-server fd */
            for (i=1;i < MAX_CONNECTIONS;i++) {
                if ((all_connections[i] > 0) &&
                    (FD_ISSET(all_connections[i], &read_fd_set))) {

                    /* read incoming data */
                    memset(buf, '\0', DATA_BUFFER);
                    ret_val =  recv(all_connections[i], buf, DATA_BUFFER, 0);
                    if (ret_val == 0) {
                        printf("Goodbye connection with fd: %d\n", all_connections[i]);
                        close(all_connections[i]);
                        all_connections[i] = -1; /* Connection is now closed */

                        // Change to next connection
                        qpop(&svQueue);
                        if(!q_isEmpty(&svQueue)){
                        	fd_now = qfront(&svQueue);
                        	ret_val = send(all_connections[fd_now], SERVER_FREE, sizeof(SERVER_FREE), 0);
                        }
                        else fd_now = -1;
                        client_login = 0;
                    } 
                    if (ret_val > 0) { 
                    	// Login or Register
                        if(!client_login){
                        	if(strcmp(buf, LOGIN)==0){
                        		printf("\nSomeone try to login fd: %d", all_connections[i]);
                        		fill_buff("\nInput ID and Password\n");
                        		ret_val = send(all_connections[i], buf, sizeof(buf), 0);
                        		login(all_connections[i]);
                        	}
                        	else if(strcmp(buf, REGISTER)==0){
                        		printf("\nSomeone try to Register fd: %d\n", all_connections[i]);
                        		fill_buff("\nInput ID and Password\n");
                        		ret_val = send(all_connections[i], buf, sizeof(buf), 0);
                        		regist(all_connections[i]);
                        	}
                        	else{
                        		printf("\nWrong input from Client with fd: %d\n", all_connections[i]);
                        	}
                        }
                        // App
                        else if(client_login){
                        	if(strcmp(buf, ADD) == 0){
                        		printf("\nSomeone want to input file\n");
                        		fill_buff("Input file\n");
                        		ret_val = send(all_connections[i], buf, sizeof(buf), 0);
                        		add_file(all_connections[i]);
                        	}
                        	else if(strcmp(buf, SEE) == 0){
                        		printf("\nSomeone want to see the files on server\n");
                        		fill_buff("The files on the server are these\n");
                        		ret_val = send(all_connections[i], buf, sizeof(buf), 0);
                        		see(all_connections[i]);
                        	}
                            else if(strcmp(buf, FIND) == 0){
                                find_file(all_connections[i]);
                            }
                            else if(strcmp(buf, DOWNLOAD)==0){
                                download_file(all_connections[i]);
                            }
                            else if(strcmp(buf, DELETE)==0){
                                delete_file(all_connections[i]);
                            }
                        	else{
                        		printf("Someone came in to the app fd: %d\n", all_connections[i]);
                        		printf("Data came in: %s\n", buf);
                        		fill_buff("\nYour message was sent successfully\n");
                        		ret_val = send(all_connections[i], buf, sizeof(buf), 0);
                        	}
                        }
                    } 
                    if (ret_val == -1) {
                        printf("recv() failed for fd: %d [%s]\n", 
                            all_connections[i], strerror(errno));
                        break;
                    }
                }
                ret_val--;
                if (!ret_val) continue;
            } /* for-loop */
        } /* (ret_val >= 0) */
    } /* while(1) */

    /* Last step: Close all the sockets */
    for (i=0;i < MAX_CONNECTIONS;i++) {
        if (all_connections[i] > 0) {
            close(all_connections[i]);
        }
    }
    return 0;
}

void init_server(){
	FILE *fp0 = fopen("files.tsv", "a+");
	fclose(fp0);
	FILE *fp1 = fopen("running.log", "a+");
	fclose(fp1);
	FILE *fp2 = fopen("akun.txt", "a+");
	fclose(fp2);
	mkdir("FILES", 0777);
	// Get FILES folder realpath
	char *res = realpath("./FILES/", server_path);
	strcat(server_path, "/");
}

void fill_buff(char *message){
	memset(buf, '\0', DATA_BUFFER);
	sprintf(buf, "%s", message);
}

void login(int fd_number){
	memset(buf, '\0', DATA_BUFFER);
	ret_val = recv(fd_number, buf, DATA_BUFFER, 0);
	char id_pass[DATA_BUFFER];
	strcpy(id_pass, buf);
	if(check_akun(id_pass)){
		fill_buff(LOGIN_SUCCESS);
		client_login = true;
		strcpy(id_pass_now, id_pass);
	}
	else
		fill_buff(LOGIN_FAILED);
	ret_val = send(all_connections[i], buf, sizeof(buf), 0);
}

bool check_akun(char id_pass[]){
	char akun[DATA_BUFFER], check[DATA_BUFFER];
	// Copy id_pass to check for checking
	strcpy(check, id_pass);
	strcat(check, "\n");
	bool flag = false;
	FILE *fp = fopen("akun.txt", "r");
	while(fgets(akun, sizeof(akun), fp)){
		if(strcmp(check, akun)==0){
			flag = true;
			break;
		}
	}
	if(flag) return true;
	return false;
}

void regist(int fd_number){
	memset(buf, '\0', DATA_BUFFER);
	ret_val = recv(fd_number, buf, DATA_BUFFER, 0);
	char id_pass[DATA_BUFFER];
	strcpy(id_pass, buf);
	save_akun(id_pass);
	strcpy(id_pass_now, id_pass);
	fill_buff("Register success\n");
	ret_val = send(fd_number, buf, sizeof(buf), 0);
	client_login = true;
}

void save_akun(char id_pass[]){
	FILE *fp = fopen("akun.txt", "a");
	fprintf(fp, "%s\n", id_pass);
	fclose(fp);
}

void add_file(int fd_number){
	char publisher[DATA_BUFFER], tahun[DATA_BUFFER], file[DATA_BUFFER], filepath[MAX/2];

	// Publisher, Tahun, File
	ret_val = recv(fd_number, publisher, DATA_BUFFER, 0);
	ret_val = recv(fd_number, tahun, DATA_BUFFER, 0);
	ret_val = recv(fd_number, file, DATA_BUFFER, 0);

	// Ambil nama file
	char filename[100];
	take_file_name(filename, file);

	// Inisiasi filepath saat di server
	strcpy(filepath, server_path);
	strcat(filepath, filename);

	// Save file to files.tsv
	save_to_tsv(publisher, tahun, filepath);

	// Send file to server
	char filecontent[DATA_BUFFER];
	ret_val = recv(fd_number, filecontent, DATA_BUFFER, 0);
	send_file_to_server(filepath, filecontent);

	// Log
	log_activity("Tambah", filename);

	fill_buff("File saved to server!\n");
	ret_val = send(fd_number, buf, sizeof(buf), 0);
}

void take_file_name(char filename[], char file[]){
	char *check, file_temp[DATA_BUFFER], str[100];
	strcpy(file_temp, file);
	check = strtok(file_temp, "/");
	while(check != NULL)
	{
		strcpy(str, check);
		check = strtok(NULL, "/");
	}
	strcpy(filename, str);
}

void save_to_tsv(char publisher[], char tahun[], char filepath[]){
	FILE *fp = fopen("files.tsv", "a");
	fprintf(fp, "%s\t%s\t%s\n", filepath, publisher, tahun);
	fclose(fp);
}

void send_file_to_server(char filepath[], char filecontent[]){
	FILE *fp = fopen(filepath, "w");
	fputs(filecontent, fp);
	fclose(fp);
}

void log_activity(char *status, char filename[]){
	FILE *fp = fopen("running.log", "a");
	fprintf(fp, "%s : %s(%s)\n", status, filename, id_pass_now);
	fclose(fp);
}

void see(int fd_number){
	char deskripsi_total[MAX], file[MAX];
	strcpy(deskripsi_total, "\n");
	FILE *fp = fopen("files.tsv", "r");
	while(fgets(file, sizeof(file), fp)){
		// Deskripsi sementara
		char deskripsi[MAX], *check, file_temp[MAX], filepath[100], name[100], ekstensi[100], publisher[100], tahun[100];
		

		// Ambil filename dan ekstensi
		strcpy(file_temp, file);
		char filename[1000], *dot;
		check = strtok(file_temp, "\t");
		strcpy(filepath, check);
		take_file_name(filename, check);
		dot = strtok(filename, ".");
		strcpy(name, dot);
		dot = strtok(NULL, ".");
		strcpy(ekstensi, dot);

		// Ambil publisher
		strcpy(file_temp, file);
		check = strtok(file_temp, "\t");
		check = strtok(NULL, "\t");
		strcpy(publisher, check);

		// Ambile Tahun
		check = strtok(NULL, "\t");
		strcpy(tahun, check);

		// Gabung
		sprintf(deskripsi, "Nama: %s\nPublisher: %s\nTahun Publishing: %sEkstensi File: %s\nFilepath: %s\n\n", name, publisher, tahun, ekstensi, filepath);

		// Gabung Total
		strcat(deskripsi_total, deskripsi);
	}
	fclose(fp);

	ret_val = send(fd_number, deskripsi_total, sizeof(deskripsi_total), 0);
}

void find_file(int fd_number){
    char filename[DATA_BUFFER];
    ret_val = recv(fd_number, buf, DATA_BUFFER, 0);
    strcpy(filename, buf);

    // Find file in files.tsv
    char file[100], files_list[MAX];
    strcpy(files_list, "");

    FILE *fp = fopen("files.tsv", "r");
    while(fgets(file, sizeof(file), fp)){
        char *check;
        check = strtok(file, "\t");
        if(strstr(check, filename) != NULL){
            char temp[100];
            take_file_name(temp, file);
            strcat(files_list, temp);
            strcat(files_list, "\n");
            continue;
        }
    }
    fclose(fp);

    ret_val = send(fd_number, files_list, sizeof(files_list), 0);
}

void download_file(int fd_number){
    char filename[DATA_BUFFER];
    ret_val = recv(fd_number, buf, DATA_BUFFER, 0);
    strcpy(filename, buf);

    // Check if file exist
    char filepath[MAX/2];
    strcpy(filepath, server_path);
    strcat(filepath, filename);
    FILE *fp = fopen(filepath, "rb");
    if(fp == NULL){
        fill_buff(ERROR);
        ret_val = send(fd_number, buf, sizeof(buf), 0);
    }
    else{
        fill_buff(FOUND);
        ret_val = send(fd_number, buf, sizeof(buf), 0);

        // Read file content
        fseek(fp, 0, SEEK_END);
        long fsize = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *string = malloc(fsize+1);
        fread(string, 1, fsize, fp);

        // Send file content
        fill_buff(string);
        ret_val = send(fd_number, buf, sizeof(buf), 0);
    }
    fclose(fp);
}

void delete_file(int fd_number){
    char filename[DATA_BUFFER];
    ret_val = recv(fd_number, buf, DATA_BUFFER, 0);
    strcpy(filename, buf);

    // Rename file while also check if file exist
    char oldfilename[MAX/2];
    strcpy(oldfilename, "old-");
    strcat(oldfilename, filename);

    char filepath[MAX], oldfilepath[MAX];
    strcpy(filepath, server_path);
    strcat(filepath, filename);
    strcpy(oldfilepath, server_path);
    strcat(oldfilepath, oldfilename);

    int ret = rename(filepath, oldfilepath);
    // File exist
    if(ret == 0){
        // Delete a line from files.tsv
        char line[MAX/2];
        FILE *fp = fopen("files.tsv", "r");
        FILE *tp = fopen("temp.tsv", "w+");
        while(fgets(line, sizeof(line), fp)){
            if(strstr(line, filename) == NULL)
                fprintf(tp, "%s", line);
        }
        fclose(fp);
        fclose(tp);
        remove("files.tsv");
        rename("temp.tsv", "files.tsv");

        // Log
        log_activity("Hapus", filename);

        fill_buff(SUCCESS);
        ret_val = send(fd_number, buf, sizeof(buf), 0);
    }
    else{
        fill_buff(ERROR);
        ret_val = send(fd_number, buf, sizeof(buf), 0);
    }
}