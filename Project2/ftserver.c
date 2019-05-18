#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <dirent.h>

int socket_set_up(int socket_id, struct sockaddr_in * address, int port) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(port);
    int error;
    error = bind(socket_id, (struct sockaddr *)address, sizeof(*address));
    if (error == -1) {
        printf("Port binding failed.", port);
        return 1;
    }
    error = listen(socket_id, 3);
    if (error == -1) {
        printf("Port listening failed.");
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int port_command;
    sscanf(argv[1], "%d", &port_command);
    if (port_command < 0 || port_command > 65535) {
        printf("Entered port number is invalid.\n");
        exit(1);
    }
    struct sockaddr_in address_command = {0};
    struct sockaddr_in address_file = {0};
    int addrCommandLength = sizeof(address_command);
    int addrFileLength = sizeof(address_file);
    int error;
    char message[1024] = {0};
    int serverCommand = socket(AF_INET, SOCK_STREAM, 0);
    if (serverCommand == 0) {
        printf("Socket creation failed.");
        exit(1);
    }

    error = socket_set_up(serverCommand, &address_command, port_command);
    if (error == 0) {
        printf("Server opened at %d\n", port_command);
    }
    while (1) {
        int connectionCommand = accept(serverCommand, (struct sockaddr *)&address_command, (socklen_t*)&addrCommandLength);
        if (connectionCommand != -1) {
            memset(message, '\0', 1024);
            memset(&address_file, 0, sizeof(address_file));

            error = read(connectionCommand, message, 1024);
            if (error > 0) {
                // print info to screen
                char clientIP[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &address_command.sin_addr, clientIP, INET_ADDRSTRLEN);
                char *token;
                char command[10] = {0}, fileName[32] = {0};
                int portFile;
                // walk through tokens
                token = strtok(message, " ");
                strcpy(command, token);
                if (strcmp(command, "-l") == 0) {
                    token = strtok(NULL, " ");
                    sscanf(token, "%d", &portFile);
                    printf("List of items at port number:  %d\n", portFile);
                    struct dirent *de;
                    char directoryContent[1024] = {0};
                    DIR *directory = opendir(".");
                    de = readdir(directory);
                    while (de != NULL) {
                        if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
                            strcat(directoryContent, de->d_name);
                            strcat(directoryContent, "\n");
                        }
                        de = readdir(directory);
                    }
                    closedir(directory);

                    int serverFile = socket(AF_INET, SOCK_STREAM, 0);
                    if (serverFile == 0) {
                        printf("Socket creation failed.");
                        continue;
                    }
                    if (setsockopt(serverFile, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
                        printf("Failed");
                        continue;
                    }
                    socket_set_up(serverFile, &address_file, portFile);
                    write(connectionCommand, "Socket is ready.", 5);
                    int connectionFile = accept(serverFile, (struct sockaddr *)&address_file, (socklen_t*)&addrFileLength);
                    if (connectionFile != -1) {
                        printf("Pushing the content %s: %d\n", clientIP, portFile);
                        write(connectionFile, directoryContent, strlen(directoryContent));
                    }
                    shutdown(connectionFile, 2);
                    close(connectionFile);
                    shutdown(serverFile, 2);
                    close(serverFile);
                }

                else if (strcmp(command, "-g") == 0) {
                    token = strtok(NULL, " ");
                    strcpy(fileName, token);
                    token = strtok(NULL, " ");
                    sscanf(token, "%d", &portFile);
                    printf("File \"%s\" requested on port %d\n", fileName, portFile);
                    char fileBuffer[1024];
                    FILE *f;
                    f = fopen(fileName,"r");
                    if (f == NULL) {
                        printf("Requested data not found.");
                        write(connectionCommand, "File not found.", 14);
                        continue;
                    }
                    int serverFile = socket(AF_INET, SOCK_STREAM, 0);
                    if (serverFile == 0) {
                        printf("Socket creation failed.");
                        continue;
                    }
                    if (setsockopt(serverFile, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
                        printf("Setting the socket failed.");
                        continue;
                    }
                    socket_set_up(serverFile, &address_file, portFile);
                    write(connectionCommand, "Ready\n", 5);
                    int connectionFile = accept(serverFile, (struct sockaddr *)&address_file, (socklen_t*)&addrFileLength);
                    if (connectionFile != -1) {
                        printf("Sending \"%s\" to %s: %d\n", fileName, clientIP, portFile);
                        int bytesRead;
                        while ((bytesRead = fread(fileBuffer, 1, sizeof(fileBuffer), f)) > 0)
                            write(connectionFile, fileBuffer, bytesRead);
                    }
                    shutdown(connectionFile, 2);
                    close(connectionFile);
                    shutdown(serverFile, 2);
                    close(serverFile);
                }
                else {
                    write(connectionCommand, "Command is invalid.", 15);
                }
            }
        }
    }
    return  0;
}

