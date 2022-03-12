#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <stdio.h>
#include <unistd.h>     /* read, write, close */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <arpa/inet.h>
#include "helpers.h"
#include "requests.h"

char *compute_get_request(char *host, char *url, char *query_params,
                            char **cookies,
                            char *JWTToken) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", *cookies);
        
        compute_message(message, line);
    }

    if (JWTToken != NULL){
        memset(line, 0, LINELEN);
    	sprintf(line, "Authorization: Bearer %s", JWTToken);

    	compute_message(message, line);
    }

    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(char *host, char *url, char* content_type,
                            char **body_data, int body_data_fields_count,
                            char **cookies,
                            char *JWTToken)
{
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

	memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

	memset(line, 0, LINELEN);
	sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    
    int length = 0;
    for (int i = 0 ; i < body_data_fields_count ; i++){
    	length += strlen(body_data[i]);
    }
    
    sprintf(line, "Content-Length: %d", length);
    compute_message(message, line);

    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", *cookies);
    
        compute_message(message,line);
    }

    if (JWTToken != NULL){
        memset(line, 0, LINELEN);
    	sprintf(line, "Authorization: Bearer %s", JWTToken);

    	compute_message(message, line);
    }

	compute_message(message, "");

    for (int i = 0 ; i < body_data_fields_count ; i++){
        memset(line, 0, LINELEN);
    	sprintf(line, "%s", body_data[i]);

    	compute_message(message, line);
    }

    compute_message(message, "");

    free(line);
	free(body_data_buffer);
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                            char **cookies,
                            char *JWTToken) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));

    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }
    compute_message(message, line);

    memset(line, 0, LINELEN);
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    if (cookies != NULL) {
        memset(line, 0, LINELEN);
        sprintf(line, "Cookie: %s", *cookies);
        
        compute_message(message, line);
    }

    if (JWTToken != NULL){
        memset(line, 0, LINELEN);
    	sprintf(line, "Authorization: Bearer %s", JWTToken);

    	compute_message(message, line);
    }

    compute_message(message, "");
    free(line);
    return message;
}
