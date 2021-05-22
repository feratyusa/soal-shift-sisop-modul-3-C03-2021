#include <stdio.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>

#define PORT 8080

// Server variables
int server_fd, new_socket, valread;
struct sockaddr_in address;
int opt = 1;
int addrlen = sizeof(address);

// Message variables
char *welcome = "Welcome!\n1. Login\n2. Register\n(Type the number)";
char *error_input = "Input not recognized\n";
bool status;
bool log_in;
char server_path[PATH_MAX];

// User Variable
char id_now[1024];
char pass_now[1024];

int create_tcp_server_socket(){
	struct sockaddr_in saddr;
	int fd, ret_val;

	// Step1: create a TCP socket
	server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(fd == -1){
		fprintf(stderr, "socket failed [%s]\n", strerror(errno));
		return -1;
	}
	printf("Created a socket with fd: %d\n", fd);

	// Initialize the socket address structure
	saadr.sin_family = AF_INET;
	saddr.sin_port = htons(7000);
	saddr.sin_addr.s_addr = INADDR_ANY;

	// Step2: bind the socket to port 7000 in the local host
	ret_val = bind(fd, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
	if(ret_val != 0){
		fprintf(stderr, "bind failed [%s]\n", strerror(errno));
		close(fd)
		return -1;
	}

	// Step3: listen for incoming connections
	ret_val = listen(fd,5);
	if(ret_val != 0){
		fprintf(stderr, "listen failed [%s]\n", stderror(errno));
		close(fd)
		return -1
	}
	return fd;
}

void init_server()
{
	FILE *fp0 = fopen("files.tsv", "a+");
	fclose(fp0);
	FILE *fp1 = fopen("running.log", "a+");
	fclose(fp1);
	FILE *fp2 = fopen("akun.txt", "a+");
	fclose(fp2);
	mkdir("FILES", 0777);
}

void send_to_client(char *message)
{
	send(new_socket, message, strlen(message), 0);
}

void read_from_client(char buffer[])
{
	int r;
	r = read(new_socket, buffer, 1024);
}

// Membaca user yang masuk
bool read_user(char id[], char pass[])
{
	bool flag = false;
	strcat(pass, "\n");

	FILE *fp = fopen("akun.txt", "r");
	char akun[1024];
	char id_c[1024], pass_c[1024];
	char *check;
	while(fgets(akun, sizeof(akun), fp))
	{	
		printf("%s\n",akun);
		// Take id
		check = strtok(akun, ":");
		strcpy(id_c, check);

		// Take pass
		check = strtok(NULL, ":");
		strcpy(pass_c, check);

		// Check id and pass
		if(strcmp(id, id_c) == 0 && strcmp(pass, pass_c) == 0)
		{
			flag = true;
			break;
		}
	}
	fclose(fp);

	if(flag)
	{
		printf("Login Success\n");
		return true;
	}

	printf("Login failed\n");
	return false;
}

void login()
{
	char *id_mess = "Login\nID:";
	send_to_client(id_mess);

	char id[1024] = {0};
	read_from_client(id);

	char *pass_mess = "Password:";
	send_to_client(pass_mess);

	char pass[1024] = {0};
	read_from_client(pass);

	if(read_user(id, pass))
	{
		char *valid = "You are logged in!";
		send_to_client(valid);

		strcpy(id_now, id);
		strcpy(pass_now, pass);
		status = true;
		log_in = true;
	}
	else
	{	
		printf("Invalid\n");
		char *invalid = "ID or Password is incorrect\n1. Login\n2. Register";
		send_to_client(invalid);
	}
	return;
}

void regist()
{
	char *regist = "Register\nEnter ID:";
	send_to_client(regist);

	char id[1024] = {0};
	read_from_client(id);

	char *regist2 = "Enter Password:";
	send_to_client(regist2);

	char pass[1024] = {0};
	read_from_client(pass);

	char id_pass[1024] = {0};
	strcat(id_pass,id);
	strcat(id_pass, ":");
	strcat(id_pass, pass);
	strcat(id_pass, "\n");

	// Input new user
	FILE *fp = fopen("akun.txt", "a+");
	fputs(id_pass, fp);
	fclose(fp);

	char *new = "Welcome new user!";
	send_to_client(new);

	strcpy(id_now, id);
	strcpy(pass_now, pass);
	status = true;
	log_in = true;
}

void take_filename(char filepath[], char filename[])
{
	char *check, str[100];
	check = strtok(filepath, "/");
	while(check != NULL)
	{
		strcpy(str, check);
		check = strtok(NULL, "/");
	}
	strcpy(filename, str);
}

// Revisi: Pakai socket
void copy_file(char filepath[])
{
	char cf[1024], filename[1024];
	strcpy(cf, filepath);
	take_filename(cf, filename);
	char serverfile[1024];
	strcpy(serverfile, "./FILES/");
	strcat(serverfile, filename);

	FILE *source = fopen(filepath, "r");
	FILE *dest = fopen(serverfile, "w+");

	char c;
	while((c = fgetc(source)) != EOF) fputc(c, dest);

	fclose(source);
	fclose(dest);

	return;
}

void log_activity(char status[], char filepath[])
{
	// Take filename
	char cf[1024], filename[1024];
	strcpy(cf, filepath);
	take_filename(cf, filename);
	printf("Filename: %s\n", filename);

	FILE *fp = fopen("running.log", "a");
	fprintf(fp, "%s : %s(%s:%s)\n", status, filename, id_now, pass_now);
	fclose(fp);
}

void save_to_tsv(char publisher[], char tahun[], char filepath[])
{
	char cf[1024], filename[1024];
	strcpy(cf, filepath);
	take_filename(cf, filename);
	char serverfile[1024];
	strcpy(serverfile, server_path);
	strcat(serverfile, filename);

	FILE *fp = fopen("files.tsv", "a");
	fprintf(fp, "%s\t%s\t%s\n",serverfile, publisher, tahun);
	fclose(fp);
}

void add_command()
{
	char *pub = "Publisher :";
	send_to_client(pub);
	char publisher[1024] = {0};
	read_from_client(publisher);

	char *year = "Tahun Publikasi :";
	send_to_client(year);
	char tahun[1024] = {0};
	read_from_client(tahun);

	char *f = "Filepath :";
	send_to_client(f);
	char filepath[1024] = {0};
	read_from_client(filepath);

	save_to_tsv(publisher, tahun, filepath);
	char status[7] = "Tambah";
	log_activity(status, filepath);
	copy_file(filepath);

	char *output = "Data saved.";
	send_to_client(output);
}

void download_file()
{

}

void take_ekstensi(char filename[], char ekstensi[])
{
	char *check;
	check = strtok(filename, ".");
	check = strtok(NULL, ".");
	strcpy(ekstensi, check);
}

void get_deskripsi(char deskripsi[], char isi[])
{
	
}

void see_command()
{
	FILE *fp = fopen("files.tsv", "r");
	char isi[1024], deskripsi_total[1024];
	while(fgets(isi, sizeof(isi), fp))
	{
		char deskripsi[1024];
		get_deskripsi(deskripsi, isi);
		strcat(deskripsi_total, deskripsi);
	}
	fclose(fp);

	send_to_client(deskripsi_total);
}

// Revisi: multiple clients
int main()
{
	init_server();

	server_fd = create_tcp_server_socket

	// Send welcome message
	send_to_client(welcome);
	printf("Someone came in\n");

	// Login or Register from Client
	status = false;
	log_in = false;
	while(!status)
	{
		// Read client input
		char user_input[1024] = {0};
		read_from_client(user_input);

		// Login input
		if(strcmp(user_input, "1") == 0)
		{
			printf("Someone tried to log in\n");
			login();
		}
		// Register input
		else if(strcmp(user_input, "2") == 0)
		{
			printf("Someone tried to register\n");
			regist();
		}
		// Error input
		else
		{
			send(new_socket, error_input, strlen(error_input), 0);
		}
	}

	// Get FILES path
	char *res = realpath("./FILES/", server_path);
	strcat(server_path, "/");

	if(log_in)
	{
		printf("Running app\n");

		status = false;
		while(!status)
		{
			printf("Masuk app\n");
			char command[1024] = {0};
			read_from_client(command);

			if(strcmp(command, "add") == 0)
			{
				printf("Adding file\n");
				add_command();
			}
			// Revisi : see, download, delete
			else if(strcmp(command, "see") == 0) 
			{
				printf("User sees the files\n");
				see_command();
			}
			else
			{
				status = true;
			}
		}
	}

	return 0;
}