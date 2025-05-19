#include "http_client.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>

#define HOST "api.openweathermap.org"
#define PORT 80

char *http_get(const char *lat, const char *lon, const char *key) {
    int sockfd;
    struct sockaddr_in server;
    struct hostent *he;

    if ((he = gethostbyname(HOST)) == NULL) return NULL;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr = *((struct in_addr *)he->h_addr);
    memset(&(server.sin_zero), 0, 8);

    if (connect(sockfd, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0)
        return NULL;

    char request[512];
    snprintf(request, sizeof(request),
             "GET /data/2.5/weather?lat=%s&lon=%s&appid=%s&units=metric HTTP/1.0\r\n"
             "Host: %s\r\n\r\n", lat, lon, key, HOST);
    send(sockfd, request, strlen(request), 0);

    char *response = malloc(2048);
    int total = 0, n;
    while ((n = recv(sockfd, response + total, 2048 - total, 0)) > 0) total += n;
    response[total] = '\0';

    close(sockfd);

    char *body = strstr(response, "\r\n\r\n");
    if (body) return strdup(body + 4);

    free(response);
    return NULL;
}

