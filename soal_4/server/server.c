#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>

#define PORT 5000
#define LIST "/home/kyfaiyya/modul3/soal4/myanimelist.csv"
#define LOG "/home/kyfaiyya/modul3/soal4/change.log"
#define TEMP "/home/kyfaiyya/modul3/soal4/temp.csv"

// Struktur untuk menyimpan informasi file dan log
typedef struct {
    FILE *file;
    FILE *logFile;
} FileData;

// Struktur untuk menyimpan informasi koneksi server
typedef struct {
    int serverSocket;
    int socket;
    struct sockaddr_in serverAddr;
    struct sockaddr_in clientAddr;
    socklen_t addr_size;
    int addrlen;
} ServerData;

void RequestClient(char *buffer, char *response, FileData *fileData);
void startServer(ServerData *serverData);

int main() {
    ServerData serverData;

    // Start the server
    startServer(&serverData);

    return 0;
}

void RequestClient(char *buffer, char *response, FileData *fileData) {
    // Implementation of RequestClient function
    fileData->file = fopen(LIST, "r");
    fileData->logFile = fopen(LOG, "a");
    
    char line[2048];
    char command[2048];
    char parameter[2048];
    int found = 0;

    sscanf(buffer, "%s %[^\n]", command, parameter);

    switch (command[0]) {
        case 't':
            {
                int count = 1;
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    if (line[0] != '\n') {
                        char *token = strtok(line, ",");
                        token = strtok(NULL, ",");
                        token = strtok(NULL, ",");
                        char numberedLine[2048];
                        sprintf(numberedLine, "%d. %s", count, token);
                        strcat(response, numberedLine);
                        strcat(response, "\n");
                        count++;
                    }
                }
            }
            break;
        case 'h':
            {
                rewind(fileData->file);
                int count = 1;
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    if (line[0] != '\n') {
                        char *token = strtok(line, ",");
                        if (strcmp(token, parameter) == 0) {
                            token = strtok(NULL, ",");
                            token = strtok(NULL, ",");
                            char numberedLine[2048];
                            sprintf(numberedLine, "%d. %s", count, token);
                            strcat(response, numberedLine);
                            strcat(response, "\n");
                            found = 1;
                            count++;
                        }
                    }
                }
                if (!found) {
                    strcat(response, "Tidak ada anime pada hari tersebut\n");
                }
            }
            break;
        case 'g':
            {
                rewind(fileData->file);
                int count = 1;
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    if (line[0] != '\n') {
                        char *token = strtok(line, ",");
                        token = strtok(NULL, ",");
                        if (strcmp(token, parameter) == 0) {
                            token = strtok(NULL, ",");
                            char numberedLine[2048];
                            sprintf(numberedLine, "%d. %s", count, token);
                            strcat(response, numberedLine);
                            strcat(response, "\n");
                            found = 1;
                            count++;
                        }
                    }
                }
                if (!found) {
                    strcat(response, "Tidak ada anime dengan genre tersebut\n");
                }
            }
            break;
        case 's':
            {
                rewind(fileData->file);
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    if (line[0] != '\n') {
                        char *token = strtok(line, ",");
                        token = strtok(NULL, ",");
                        token = strtok(NULL, ",");
                        if (strcmp(token, parameter) == 0) {
                            token = strtok(NULL, "\n");
                            strcat(response, token);
                            strcat(response, "\n");
                            found = 1;
                        }
                    }
                }
                if (!found) {
                    strcat(response, "Tidak ada anime dengan status tersebut\n");
                }
            }
            break;
        case 'a':
            {
                rewind(fileData->file);
                FILE *temp = fopen(TEMP, "w");
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    fputs(line, temp);
                }
                fprintf(temp, "\n%s", parameter);
                fclose(temp);
                fclose(fileData->file);
                remove(LIST);
                rename(TEMP, LIST);
                fileData->file = fopen(LIST, "r");

                time_t now = time(NULL);
                struct tm *t = localtime(&now);
                char dateStr[20];
                strftime(dateStr, sizeof(dateStr), "%d/%m/%y", t);

                char *title = strtok(parameter, ",");
                title = strtok(NULL, ",");
                title = strtok(NULL, ",");

                fprintf(fileData->logFile, "[%s] [ADD] %s ditambahkan.\n", dateStr, title);
                strcat(response, "anime berhasil ditambahkan\n");
            }
            break;
        case 'e':
            {
                rewind(fileData->file);
                FILE *temp = fopen(TEMP, "w");

                char *pastTitle = strtok(parameter, ",");
                char *postLine = strtok(NULL, "\n");

                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    char *pastLine = strdup(line);
                    char *token = strtok(line, ",");
                    token = strtok(NULL, ",");
                    token = strtok(NULL, ",");
                    char *title = token;

                    if (strcmp(title, pastTitle) != 0) {
                        fputs(pastLine, temp);
                    } else {
                        fprintf(temp, "%s\n", postLine);

                        time_t now = time(NULL);
                        struct tm *t = localtime(&now);
                        char dateStr[20];
                        strftime(dateStr, sizeof(dateStr), "%d/%m/%y", t);

                        fprintf(fileData->logFile, "[%s] [EDIT] %s diubah menjadi %s.\n", dateStr, pastLine, postLine);
                    }

                    free(pastLine);
                }

                fclose(temp);
                fclose(fileData->file);
                remove(LIST);
                rename(TEMP, LIST);
                fileData->file = fopen(LIST, "r");
                strcat(response, "anime berhasil diedit\n");
            }
            break;
        case 'd':
            {
                rewind(fileData->file);
                FILE *temp = fopen(TEMP, "w");
                char *title = parameter;
                while (fgets(line, sizeof(line), fileData->file) != NULL) {
                    char *pastLine = strdup(line);
                    char *titleLine = strtok(line, ",");
                    titleLine = strtok(NULL, ",");
                    titleLine = strtok(NULL, ",");

                    if (strcmp(titleLine, title) != 0) {
                        fputs(pastLine, temp);
                    } else {
                        time_t now = time(NULL);
                        struct tm *t = localtime(&now);
                        char dateStr[20];
                        strftime(dateStr, sizeof(dateStr), "%d/%m/%y", t);

                        fprintf(fileData->logFile, "[%s] [DEL] %s berhasil dihapus.\n", dateStr, title);
                    }
                }
                fclose(temp);
                fclose(fileData->file);
                remove(LIST);
                rename(TEMP, LIST);
                fileData->file = fopen(LIST, "r");
                strcat(response, "anime berhasil dihapus\n");
            }
            break;
        default:
            strcat(response, "Invalid Command\n");
    }

    fclose(fileData->file);
    fclose(fileData->logFile);
}

void startServer(ServerData *serverData) {
    serverData->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverData->serverSocket < 0) {
        printf("Error creating socket\n");
        exit(EXIT_FAILURE);
    }

    serverData->serverAddr.sin_family = AF_INET;
    serverData->serverAddr.sin_port = htons(PORT);
    serverData->serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverData->serverSocket, (struct sockaddr *)&(serverData->serverAddr), sizeof(serverData->serverAddr)) < 0) {
        printf("Error binding socket\n");
        exit(EXIT_FAILURE);
    }

    if (listen(serverData->serverSocket, 1) < 0) {
        printf("Error listening\n");
        exit(EXIT_FAILURE);
    }

    while (1) {
        serverData->addr_size = sizeof(serverData->clientAddr);
        serverData->socket = accept(serverData->serverSocket, (struct sockaddr *)&(serverData->clientAddr), &(serverData->addr_size));
        if (serverData->socket < 0) {
            printf("Error accepting connection\n");
            continue;
        }

        FileData fileData;
        fileData.file = fopen(LIST, "r");
        fileData.logFile = fopen(LOG, "a");

        while (1) {
            char buffer[2048];
            char response[4096];

            memset(buffer, 0, sizeof(buffer));
            recv(serverData->socket, buffer, sizeof(buffer), 0);

            printf("received: %s\n", buffer);
            if (strcmp(buffer, "exit") == 0) {
                printf("Exiting the client\n");
                exit(0);
            }
            memset(response, 0, sizeof(response));
            RequestClient(buffer, response, &fileData);
            send(serverData->socket, response, strlen(response), 0);
        }
        fclose(fileData.file);
        fclose(fileData.logFile);
        close(serverData->socket);
    }
    close(serverData->serverSocket);
}
