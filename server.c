#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "utils.h"

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// This will be filled up later
struct client list_of_all_clients[MAX_CLIENTS] = {0};
struct session list_of_all_active_sessions[MAX_SESSIONS] = {0};

int find_index_of_client(char *client_id) {
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (list_of_all_clients[i] == 0) {
      continue;
    } 
    else {
      if (!strcmp(client_id, list_of_all_clients[i].client_id)) {
        return i;
      }
    }
  }
  return -1;
}

int find_index_of_session(char* session_id){
    for (int i = 0; i < MAX_SESSIONS; i++) {
        if (list_of_all_active_sessions[i] == 0) {
          continue;
        } 
        else {
          if (!strcmp(session_id, list_of_all_active_sessions[i].session_id)) {
            return i;
          }
        }
      }
      return -1;
    }

void create_message(struct message message_to_send, char *buf) {
  sprintf(buf, "%s:%s:%s:%s", itoa(message_to_send.type),
          itoa(message_to_send.size), message_to_send.source,
          message_to_send.type);
  return;
}

int login_type(struct message message_received, int sockfd) {

  pthread_mutex_lock(&clients_mutex);
  int index_of_client = find_index_of_client(message_received.source);
  pthread_mutex_unlock(&clients_mutex);

  // Mutex unlock I guess?

  if (index_of_client == -1) {
    // Send back that username does not exist
    struct message message_to_send = {.type = LO_NAK,
                                      .size = strlen("Username does not exist"),
                                      // terminating character?
                                      .source = "",  // Not sure about this
                                      .data = "Username does not exist"};
    char buf[BUFFER_SIZE];
    create_message(message_to_send, buf);
    send(sockfd, buf, strlen(buf), 0);
    return -1;
  }

  
  pthread_mutex_lock(&clients_mutex); // Unsure of this
  if (list_of_all_clients[index_of_client].active) {
    // Send back that the use is already active
    struct message message_to_send = {
        .type = LO_NAK,
        .size = strlen("Username is already logged in"),
        // terminating character?
        .source = "",  // Not sure about this
        .data = "Username is already logged in"};

    char buf[BUFFER_SIZE];
    create_message(message_to_send, buf);
    pthread_mutex_unlock(&clients_mutex); // Perhaps might have to place this AFTER the send()?
    send(sockfd, buf, strlen(buf), 0);
    return -1;
  }
  // Send back a LOG_ACK packet and change active status of user

  struct message message_to_send = {.type = LO_ACK,
                                    .size = strlen("Login Successful"),
                                    .source = "",  // Not sure about this
                                    .data = "Login Successful"};
  char buf[BUFFER_SIZE];
  create_message(message_to_send, buf);
  list_of_all_clients[index_of_client].active = true;
  pthread_mutex_unlock(&clients_mutex);  // Perhaps might have to place this AFTER the send()?

  send(sockfd, buf, strlen(buf), 0);
  return 0;
}

void exit_type(struct message message_received, int sockfd){
    // Perhaps check if all sessions they are in are quit or not? 
    pthread_mutex_lock(clients_mutex);
    list_of_all_clients[index_of_client].active = false;
    pthread_mutex_unlock(clients_mutex);
    close(sockfd);
    pthread_exit(NULL);

}

void join_session(struct message message_received, int sockfd){
    /* GENERAL OVERVIEW    :    Find the session with the corresponding session id, and this client to the session. */

    pthread_mutex_lock(&clients_mutex);
    int index_of_session = find_index_of_session(message_received.data);
    pthread_mutex_unlock(&clients_mutex);

    


}

void leave_session(struct message message_received, int sockfd){}

void new_session(struct message message_received, int sockfd){}





void *client_handler(void *client_fd_pt) {
  int client_fd = *(int *)client_fd_pt;
  free(client_fd_pt);
  bool logged_in = false;

  while (1) {
    // recv() call
    struct message message_received = {0};
    // Process_message call

    if (!logged_in) {
      if (login_call(message_received, client_fd) != -1) {
        logged_in = true;
      }
      continue;
    }

    switch (message_received.type)
    {
    case EXIT:
        exit_type(message_received, sockfd);
        break;
    
    default:
        break;
    }

  }

}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Insufficient arguments for %s", argv[0]);
    exit(EXIT_FAILURE);
  }

  int sockfd, new_fd;
  struct addrinfo hints, *servinfo, *p;
  struct sockaddr_storage their_addr;
  socklen_t sin_size;
  int yes = 1;
  char s[INET6_ADDRSTRLEN];
  int rv;

  memset(&hints, 0, sizeof hints);

  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if ((rv = getaddrinfo(NULL, argv[1], &hints, &servinfo)) != 0) {
    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
    return 1;
  }

  for (p = servinfo; p != NULL; p = p->ai_next) {
    if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      perror("server: socket");
      continue;
    }

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
    }

    if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
      close(sockfd);
      perror("server: bind");
      continue;
    }

    break;
  }

  freeaddrinfo(servinfo);  // all done with this structure

  if (p == NULL) {
    fprintf(stderr, "server: failed to bind\n");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }

  printf("server: waiting for connections...\n");

  while (1) {
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      perror("accept");
      continue;
    }

    // Test send btw:
    if (send(new_fd, "Hello, world!", 13, 0) == -1) {
      perror("send");
    }

    pthread_t thread;
    int *new_socket_pointer = malloc(sizeof(int *));
    *new_socket_pointer = new_fd;
    pthread_create(&thread, NULL, client_handler, (void *)new_socket_pointer);
    pthread_detach(thread);
  }
}
