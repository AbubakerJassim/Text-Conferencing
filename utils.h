#ifndef UTILS
#define UTILS

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 100
#define MAX_DATA 2000
#define BUFFER_SIZE 2000
#define MAX_CLIENTS_IN_SESSION 10
#define MAX_CLIENTS 100
#define MAX_SESSIONS 10
#define BACKLOG 10

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

struct client {
  char client_id[MAX_NAME];    // USERNAME
  char password[BUFFER_SIZE];  // Should change max from MAX_NAME to BUFFER_SIZE
                               // but it doesn't really matter
  int sockfd;
  bool active;

  // Perhaps might add a linked list/array of all the sessions they are joined
  // in?
};

struct session {
  char session_id[MAX_NAME];
  struct client clients_in_session[MAX_CLIENTS_IN_SESSION];
  // An array of Client
};

struct message {
  PacketType type;
  unsigned int size;
  char source[MAX_NAME];
  char data[MAX_DATA];
};

int process_message(struct message* message_received, char* buf);

// PacketType get_packet_type(char *type);

#endif
