#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"
#include "parson.h"

char *cookie;
char *token;

int printResponseCode(char code, const char* command) {
	if (code == '4') {
		printf("Server error.\n");

		return -1;
	} else if (code == '5') {
		printf("Client error.\n");

		return -1;
	} else if (code == '2') {
		printf("Command %s went successfully.\n", command);

		return 1;
	}

	return -1;
}

void register_user() {
	int socket;
	char *message, *response;
	char username[50];
	char password[50];

	printf("username=");
	fgets(username, 50, stdin);
	username[strlen(username) - 1] = '\0';

	printf("password=");
	fgets(password, 50, stdin);
	password[strlen(password) - 1] = '\0';

	JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    char *serialized_string = NULL;

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    serialized_string = json_serialize_to_string_pretty(root_value);
    
	socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
	DIE(socket < 0, "register_user open_connection");
	
	message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/register",
						"application/json",
						&serialized_string, 1,
						NULL,
						NULL);

	send_to_server(socket, message);

    response = receive_from_server(socket);
	
	if (printResponseCode(response[9], "register") == -1)
		return;
	
    json_free_serialized_string(serialized_string);
    json_value_free(root_value);
	
	close_connection(socket);
}

void login_user(int *login) {
	int socket;
	char *message, *response;
	char username[50];
	char password[50];

	printf("username=");
	fgets(username, 50, stdin);
	username[strlen(username) - 1] = '\0';
	
	printf("password=");
	fgets(password, 50, stdin);
	password[strlen(password) - 1] = '\0';

	JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    char *serialized_string = NULL;

    json_object_set_string(root_object, "username", username);
    json_object_set_string(root_object, "password", password);

    serialized_string = json_serialize_to_string_pretty(root_value);
    
	socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
	DIE(socket < 0, "login_user open_connection");
	
    message = compute_post_request("34.118.48.238", "/api/v1/tema/auth/login",
						"application/json", 
						&serialized_string, 1,
						NULL,
						NULL);
    
	send_to_server(socket, message);

	json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    response = receive_from_server(socket);	

	if (printResponseCode(response[9], "login") == -1) {
		*login = 0;
		return;
	}

	/* Extrage Cookie-ul */
	response = strstr(response, "connect.sid=");
	response = strtok(response, "\r\n");
	
	strcpy(cookie, response);
	cookie[strlen(cookie)] = '\0';

	close_connection(socket);
}

void enter_library() {
    int socket;
	char *message, *response;
    
	socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
	DIE(socket < 0, "enter_library open_connection");
	
    message = compute_get_request("34.118.48.238", "/api/v1/tema/library/access", NULL,
								&cookie, NULL);
    
	send_to_server(socket, message);

    response = receive_from_server(socket);	

	if (printResponseCode(response[9], "enter_library") == -1)
		return;

	/* Extrage JWT Token-ul */
	response = strstr(response, "token");
	response = strtok(response, "\r\n");
	response += 8;

	strcpy(token, response);
	token[strlen(token) - 2] = '\0';

	close_connection(socket);
}

void add_book() {
	int socket;
	char *message, *response;
	char title[50], author[50], publisher[50], genre[50];
	char page_count[10];

	printf("title=");
	fgets(title, 50, stdin);
	title[strlen(title) - 1] = '\0';
	
	printf("author=");
	fgets(author, 50, stdin);
	author[strlen(author) - 1] = '\0';

	printf("publisher=");
	fgets(publisher, 50, stdin);
	publisher[strlen(publisher) - 1] = '\0';

	printf("genre=");
	fgets(genre, 50, stdin);
	genre[strlen(genre) - 1] = '\0';
	
	printf("page_count=");
	fgets(page_count, 50, stdin);
	page_count[strlen(page_count) - 1] = '\0';

	JSON_Value *root_value = json_value_init_object();
    JSON_Object *root_object = json_value_get_object(root_value);

    char *serialized_string = NULL;

    json_object_set_string(root_object, "title", title);
    json_object_set_string(root_object, "author", author);
    json_object_set_string(root_object, "publisher", publisher);
	json_object_set_string(root_object, "genre", genre);
	json_object_set_number(root_object, "page_count", atoi(page_count));

	serialized_string = json_serialize_to_string_pretty(root_value);
    
	socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
	DIE(socket < 0, "add_book open_connection");
	
    message = compute_post_request("34.118.48.238", "/api/v1/tema/library/books",
						"application/json", 
						&serialized_string, 1,
						NULL,
						token);
    
	send_to_server(socket, message);

	json_free_serialized_string(serialized_string);
    json_value_free(root_value);

    response = receive_from_server(socket);	
	
	if (printResponseCode(response[9], "add_book") == -1)
		return;

	close_connection(socket);
}

void delete_book() {
	int socket;
	char *message, *response;
	char id[10];
	
	printf("id=");
	fgets(id, 50, stdin);
	id[strlen(id) - 1] = '\0';

	char* path = (char *) calloc(BUFLEN, sizeof(char));
	strcpy(path, "/api/v1/tema/library/books/");
	strcat(path, id);
    
	socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
	DIE(socket < 0, "delete_book open_connection");

    message = compute_delete_request("34.118.48.238", path,
						"application/json", 
						NULL,
						token);
    
	send_to_server(socket, message);
    response = receive_from_server(socket);
    
	if (printResponseCode(response[9], "delete_book") == -1)
		return;

	close_connection(socket);
}

void get_books() {
	int socket;
	char *message, *response;
    
	socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
	DIE(socket < 0, "get_books open_connection");
	
    message = compute_get_request("34.118.48.238", "/api/v1/tema/library/books", NULL,
								NULL, token);

	send_to_server(socket, message);

    response = receive_from_server(socket);

	char *jsonStart = strstr(response, "[");
	if (jsonStart != NULL) printf("\nThe books are %s.\n\n", jsonStart);

	if (printResponseCode(response[9], "get_books") == -1)
		return;

	close_connection(socket);
}

void get_book() {
	int socket;
	char *message, *response;
	char id[10];
	
	printf("id=");
	fgets(id, 50, stdin);
	id[strlen(id) - 1] = '\0';

	char* path = (char *) calloc(BUFLEN, sizeof(char));
	strcpy(path, "/api/v1/tema/library/books/");
	strcat(path, id);
    
	socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
	DIE(socket < 0, "get_book open_connection");
	
    message = compute_get_request("34.118.48.238", path,
						"application/json", 
						NULL,
						token);

	send_to_server(socket, message);

    response = receive_from_server(socket);
	char *jsonStart = strstr(response, "[");
	if (jsonStart != NULL) printf("\nThe book is %s.\n\n", jsonStart);
	
	if (printResponseCode(response[9], "get_book") == -1)
		return;

	close_connection(socket);
}

void logout_user() {
    int socket;
	char *message, *response;
    
	socket = open_connection("34.118.48.238", 8080, AF_INET, SOCK_STREAM, 0);
	DIE(socket < 0, "logout_user open_connection");

    message = compute_get_request("34.118.48.238", "/api/v1/tema/auth/logout",
						"application/json", 
						&cookie,
						NULL);
    
	send_to_server(socket, message);

    response = receive_from_server(socket);
	if (printResponseCode(response[9], "logout") == -1)
		return;

	memset(cookie, 0, BUFLEN);
	memset(token, 0, BUFLEN);
	
	close_connection(socket);
}

/**
 * variabila login ca sa ma asigur ca nu se poate executa
 * o comanda care are legatura cu biblioteca decat daca
 * userul este logged in 
 * 
 * la fel si pentru register si login
 */
int executeCommand(char *command, int *login) {
	if (*login == 0) {
		if (strcmp(command, "register") == 0) {
			register_user();
			return 1;
		}

		if (strcmp(command, "login") == 0) {
			*login = 1;
			/* login_user are ca parametru functia asta
			pentru ca daca logarea mearge prost atunci
			variabil login trebuie sa fie egala cu 0 */
			login_user(login);
			return 1;
		}
	}

	if (*login == 1) {
		if (strcmp(command, "enter_library") == 0) {
			enter_library();
			return 1;
		}

		if (strcmp(command, "get_books") == 0) {
			get_books();
			return 1;
		}

		if (strcmp(command, "add_book") == 0) {
			add_book();
			return 1;
		}

		if (strcmp(command, "get_book") == 0) {
			get_book();
			return 1;
		}

		if (strcmp(command, "delete_book") == 0) {
			delete_book();
			return 1;
		}

		if (strcmp(command, "logout") == 0) {
			*login = 0;

			logout_user();
			return 1;
		}
	}

	if (strcmp(command, "exit") == 0) {
		return 0;
	}

	fprintf(stderr, "Error: wrong command entered.\n");
	return 1;
}

int main(int argc, char *argv[]) {
	char buffer[50];

	cookie = (char *) calloc(BUFLEN, sizeof(char));
	token = (char *) calloc(BUFLEN, sizeof(char));
	
	int login = 0;
	while (fgets(buffer, 60, stdin) != NULL) {
		/* Scap de newline-ul de la final */
		buffer[strlen(buffer) - 1] = '\0';
		
		if (executeCommand(buffer, &login) == 0)
			break;
	}

	free(cookie);
	free(token);

	return 0;
}
