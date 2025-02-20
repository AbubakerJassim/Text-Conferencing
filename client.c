#include <asm-generic/socket.h>
#include <bits/time.h>
#include <bits/types/struct_timeval.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <math.h>



int main(void) {
    char logInInfo[1000];
    char userCommand[50];
    char userID[100];
    char password[100];
    char serverIP[100];
    char serverResponse[1000];
    int serverPort;

    while (1) {
        printf("Welcome User to the Text Conferencing application\n");
        printf("Login by following the format (/login <client ID> <password> <server-IP> <server-port>): ");
        fgets(logInInfo, 1000, stdin);
        logInInfo[strcspn(logInInfo, "\n")] = '\0';
        int result = sscanf(logInInfo, "%s %s %s %s %d",  userCommand, userID, password, serverIP, &serverPort );

        if(strcmp(userCommand, "/login")!=0) {
            printf("ERROR: USER LOGIN COMMAND IS EITHER INCORRECT OR USER DID NOT TRY TO LOGIN.\n");
            printf("PLEASE TRY AGAIN \n\n");

        } else {
            int socketNetworkFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
            if(socketNetworkFileDescriptor==-1) {
                printf("ERROR: COULD NOT ESTABLISH A PROPER TCP CONNECTION.\n");
                return 0;

            }

            struct sockaddr_in serverAddress;
            serverAddress.sin_family = AF_INET;
            serverAddress.sin_port = htons(serverPort);
            int isIPCorrect = inet_pton(AF_INET, serverIP, &serverAddress.sin_addr);

            if(isIPCorrect!=1) {
                printf("ERROR: SERVER IP ADDRESS IS NOT CORRECT. TRY AGAIN.\n\n");
                continue;
            }

            /* REMOVE WHEN SERVER IS ABLE TO TEST
            if(connect(socketNetworkFileDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress))==-1) {
                printf("ERROR: CONNECTION TO REMOTE SOCKET FAILED.\n\n");
                continue;

            }
            
            send(socketNetworkFileDescriptor, userID, strlen(userID), 0);
            int responseBytes = recv(socketNetworkFileDescriptor, serverResponse, sizeof(serverResponse),0);
            serverResponse[responseBytes] = '\0';

            if(strcmp(serverResponse, "FAIL")!=0) {
                printf("ERROR: INVALID USER ID. TRY AGAIN.\n\n");
                continue;
            }

            send(socketNetworkFileDescriptor, password, strlen(password), 0);
            int passwordCheck = recv(socketNetworkFileDescriptor, serverResponse, sizeof(serverResponse),0);
            serverResponse[passwordCheck] = '\0';

            if(strcmp(serverResponse, "FAIL")!=0) {
                printf("ERROR: INVALID USER PASSWORD. TRY AGAIN.\n\n");
                continue;

            }

            */

            printf("%s\n %s\n %s\n %s\n %d\n", userCommand, userID, password, serverIP, serverPort);

            break;
        }
    }


}