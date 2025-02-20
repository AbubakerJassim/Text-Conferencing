#ifndef UTILS
#define UTILS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME 
#define MAX_DATA 

typedef enum {
    LOGIN, LO_ACK, LO_NAK, EXIT, JOIN, JN_ACK, JN_NAK,
    LEAVE_SESS, NEW_SESS, NS_ACK, MESSAGE, QUERY, QU_ACK, INVALID
} PacketType;

struct message {
    PacketType type;
    unsigned int size;
    char source[MAX_NAME];
    char data[MAX_DATA];
};

void process_message(struct message* message_received, char* buf);
PacketType get_packet_type(char *type);


#endif 
