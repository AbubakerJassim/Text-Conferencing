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
#include <stdbool.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


sem_t semaphore;
pthread_mutex_t lock;
bool Error = false;

typedef enum {
  LOGIN,
  LO_ACK,
  LO_NAK,
  EXIT,
  JOIN,
  JN_ACK,
  JN_NAK,
  LEAVE_SESS,
  NEW_SESS,
  NS_ACK,
  MESSAGE,
  QUERY,
  QU_ACK,
  INVALID,
  REGISTER,
  REGISTER_ACK,
  REGISTER_NACK,
  PRIVATE_MSG,
  PRIVATE_MSG_ACK,
  PRIVATE_MSG_NACK
} PacketType;

void* listenToServer(void* socketNetworkFileDescriptor) {
    while(1) {

        char serverResponse[1000];
        memset(serverResponse, 0, sizeof(serverResponse));
        
        
        int bytes = recv(*(int*)socketNetworkFileDescriptor, serverResponse, sizeof(serverResponse),0);

        if(bytes<=0) {
            sem_post(&semaphore);
            pthread_exit(NULL);
        }
        
        int valueOfType;
        int packet_size;

        //extracting the type of packet that I recieved
        char* firstColon = strchr(serverResponse, ':');
        int size = firstColon - serverResponse + 1;
        char* type = (char*)malloc(size*sizeof(char));
        strncpy(type, serverResponse, size-1);
        type[size-1] = '\0';


        sscanf(type, "%d", &valueOfType);

        //Extracting the Size of the packet
        firstColon++;
        char* secondColon = strchr(firstColon, ':');
        int length = secondColon - firstColon + 1;
        char* packetSize = (char*)malloc(length*sizeof(char));
        strncpy(packetSize, firstColon, length-1);
        packetSize[length-1] = '\0';

        sscanf(packetSize, "%d", &packet_size);

        //Extracting the source ID
    
        char* packetSource = NULL;
        if((valueOfType==MESSAGE)||(valueOfType==PRIVATE_MSG)) {
            secondColon++;
            char* thirdColon = strrchr(serverResponse, ':');
            int sourceLength = thirdColon - secondColon + 1;
            packetSource =  (char*)malloc(sourceLength*sizeof(char));
            strncpy(packetSource, secondColon, sourceLength-1);
            packetSource[sourceLength-1] = '\0';
        }

        //Extracting Packet Data
        char Data[1000];
        memset(Data, 0, sizeof(Data));

        if(packet_size!=0) {
    
            strncpy(Data, &serverResponse[strlen(serverResponse)-packet_size], packet_size);
            Data[packet_size] = '\0';
        }        

        if(valueOfType==JN_NAK) {
            printf("ERROR: %s\n", Data);
            fflush(stdout);
            sem_post(&semaphore);
            
        } else if (valueOfType==JN_ACK) {
            printf("SUCCESS: You have successfully joined the session with ID %s\n", Data);
            fflush(stdout);
            sem_post(&semaphore);

        } else if(valueOfType==NS_ACK) {
            sem_post(&semaphore);

        } else if((valueOfType==MESSAGE)||(valueOfType==PRIVATE_MSG)) {
            if(valueOfType==PRIVATE_MSG) {
                printf("(pMessage) From User %s: %s\n", packetSource, Data);

            } else {
                printf("From User %s: %s\n", packetSource, Data);
            }     

        } else if (valueOfType==QU_ACK) {
            printf("%s\n", Data);
            sem_post(&semaphore);

        } else if (valueOfType==PRIVATE_MSG_NACK) {
            printf("ERROR: %s\n", Data);
            sem_post(&semaphore);

        } else if (valueOfType==PRIVATE_MSG_ACK) {
            sem_post(&semaphore);
        }
        

        if(packetSource!=NULL) {
            free(packetSource);
        }
        free(type);
        free(packetSize);
        memset(serverResponse, 0, 1000);
        

    }
    
    return NULL;
}

int main(void) {
    char logInInfo[1000];
    char userCommand[50];
    char userID[100];
    char password[100];
    char serverIP[100];
    char serverResponse[1000];
    int serverPort;
    bool login = false;
    int socketNetworkFileDescriptor;

    do {
        memset(logInInfo, 0, sizeof(logInInfo));
        memset(userCommand, 0, sizeof(userCommand));
        memset(userID, 0, sizeof(userID));
        memset(password, 0, sizeof(password));
        memset(serverIP, 0, sizeof(serverIP));
        memset(serverResponse, 0, sizeof(serverResponse));

        printf("Welcome User to the Text Conferencing application\n");
        printf("Login or Register by following the format (/login (/register) <client ID> <password> <server-IP> <server-port>): ");
        fgets(logInInfo, 1000, stdin);
        logInInfo[strcspn(logInInfo, "\n")] = '\0';
        int result = sscanf(logInInfo, "%s %s %s %s %d",  userCommand, userID, password, serverIP, &serverPort );

        if((strcmp(userCommand, "/login")!=0) && (strcmp(userCommand, "/register")!=0)) {
            printf("ERROR: USER LOGIN COMMAND IS EITHER INCORRECT OR USER DID NOT TRY TO LOGIN.\n");
            printf("PLEASE TRY AGAIN \n\n");

        } else {
            socketNetworkFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
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

            

            if(connect(socketNetworkFileDescriptor, (struct sockaddr*) &serverAddress, sizeof(serverAddress))==-1) {
                printf("ERROR: CONNECTION TO REMOTE SOCKET FAILED.\n\n");
                continue;

            }
            


            

            char clientUserInfo[1000];
            memset(clientUserInfo, 0, 1000);

            if(strcmp(userCommand, "/register")==0) {
                sprintf(clientUserInfo, "%d:%d:%s:%s", REGISTER, (int)strlen(password), userID, password);            

            } else {
                sprintf(clientUserInfo, "%d:%d:%s:%s", LOGIN, (int)strlen(password), userID, password);            

            }

            send(socketNetworkFileDescriptor, clientUserInfo, strlen(clientUserInfo), 0);
            

            int responseBytes = recv(socketNetworkFileDescriptor, serverResponse, 2000,0);


            if((serverResponse[1]!=':') || (serverResponse[0]!='1')) {
                if((serverResponse[0]=='1') && (serverResponse[1]=='5')) {
                    printf("SUCCESS: You have successfully registered into the system\n");

                } else {
                    char* errorMessage = strrchr(serverResponse, ':') + 1;
                    printf("ERROR: %s.\n\n", errorMessage);

                }
                close(socketNetworkFileDescriptor);
                continue;
            } 

            
            


            login = true;
            fflush(stdout);
            printf("SUCCESS: Login was successful.\n\n");
            
        }



        char clientDataSequence[1000];
        char command[50];
        char sequenceID[100];
        char clientData[1000];
        pthread_t serverListener;

        if(login) {
            pthread_create(&serverListener, NULL, listenToServer, &socketNetworkFileDescriptor);
            pthread_detach(serverListener);
            sem_init(&semaphore, 0, 0);

        }
                

        while(login) {
            memset(clientDataSequence, 0, sizeof(clientDataSequence));
            memset(command, 0, sizeof(command));
            memset(sequenceID, 0, sizeof(sequenceID));
            memset(clientData, 0, sizeof(clientData));
            fgets(clientDataSequence, 1000, stdin);
            clientDataSequence[strcspn(clientDataSequence, "\n")] = '\0';
            int results = sscanf(clientDataSequence, "%s %[^\n]", command, sequenceID);
            if(results==0) {
                printf("Error: You did not enter anything. Try Again \n");
                continue;
            }
            
            if(strcmp(command, "/logout")==0) {
                memset(clientData, 0, sizeof(clientData));
                sprintf(clientData, "%d:%d:%s:",EXIT,0, userID);

                send(socketNetworkFileDescriptor,clientData, strlen(clientData), 0);
                close(socketNetworkFileDescriptor);
                sem_wait(&semaphore);
                login = false;
                printf("\n\n");
            } else if(strcmp(command, "/quit")==0) {
                sprintf(clientData, "%d:%d:%s:",EXIT,0, userID);

                send(socketNetworkFileDescriptor,clientData, strlen(clientData), 0);
                exit(0);
            } else if(strcmp(command, "/joinsession")==0) {
                sprintf(clientData, "%d:%d:%s:%s", JOIN, (int)strlen(sequenceID), userID, sequenceID);

                send(socketNetworkFileDescriptor,clientData, strlen(clientData), 0);
                sem_wait(&semaphore);
             
            } else if(strcmp(command, "/leavesession")==0) {
                sprintf(clientData, "%d:%d:%s:", LEAVE_SESS, 0, userID);

                send(socketNetworkFileDescriptor,clientData, strlen(clientData), 0);
            } else if (strcmp(command, "/createsession")==0) {
                sprintf(clientData, "%d:%d:%s:%s", NEW_SESS, (int)strlen(sequenceID), userID, sequenceID);

                send(socketNetworkFileDescriptor,clientData, strlen(clientData), 0);
                sem_wait(&semaphore);
                printf("SUCCESS: you have successfully created a session with ID %s\n", sequenceID);
            
            } else if(strcmp(command, "/list")==0) {
                sprintf(clientData, "%d:%d:%s:", QUERY, 0, userID);

                send(socketNetworkFileDescriptor,clientData, strlen(clientData), 0);
                sem_wait(&semaphore);
    
            }  else if(strcmp(command, "/pmessage")==0) {
                char* messageTo = strchr(sequenceID, ' ');

                char pMessageTo[100];
                memset(pMessageTo, 0, sizeof(pMessageTo));

                memcpy(pMessageTo, sequenceID, messageTo-sequenceID);
                sprintf(clientData, "%d:%d:%s %s:%s", PRIVATE_MSG, (int)strlen(messageTo+1), userID, pMessageTo, messageTo+1);

                send(socketNetworkFileDescriptor,clientData, strlen(clientData), 0);
                sem_wait(&semaphore);
            
            } else {
                sprintf(clientData, "%d:%d:%s:%s %s", MESSAGE, (int)strlen(command) + (int)strlen(sequenceID)+1, userID, command,sequenceID);

                send(socketNetworkFileDescriptor,clientData, strlen(clientData), 0);

            }

            
        }
    } while (!login);

} 




