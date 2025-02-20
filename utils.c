#include "utils.h"

int process_message(struct message* message_received, char* buf) {
  printf("Within process packet\n");
  int start_index = 0;
  int end_index = 0;
  char temp_buf[BUFFER_SIZE];

  // type
  while (buf[end_index] != ':' && buf[end_index] != '\0') {
    end_index++;
  }

  if (buf[end_index] == '\0') {
    return -1;
  }

  memcpy(temp_buf, buf + start_index, end_index - start_index);
  temp_buf[end_index - start_index] = '\0';
  message_received->type = atoi(temp_buf);
  start_index = ++end_index;
  printf("Message type is: %d\n", message_received->type);

  // size
  while (buf[end_index] != ':' && buf[end_index] != '\0') {
    end_index++;
  }

  if (buf[end_index] == '\0') {
    return -1;
  }

  memcpy(temp_buf, buf + start_index, end_index - start_index);
  temp_buf[end_index - start_index] = '\0';
  message_received->size = atoi(temp_buf);
  start_index = ++end_index;
  printf("Message size is: %d\n", message_received->size);

  // source
  while (buf[end_index] != ':' && buf[end_index] != '\0') {
    end_index++;
  }

  if (buf[end_index] == '\0') {
    return -1;
  }
  // Should I return -1/check if len(source) > MAX_NAME?

  memcpy(message_received->source, buf + start_index, end_index - start_index);
  message_received->source[end_index - start_index] = '\0';
  start_index = ++end_index;
  printf("Message source is: %d\n", message_received->source);

  // data
  memcpy(message_received->data, buf + start_index, message_received->size);

  // Should I return -1/check if len(data) > MAX_DATA?

  // Should I null-terminate message_received->data ?
  // message_received->data[message_received->size] = '\0';

  return 0;
}




// Might not need this function

/*
PacketType get_packet_type(char* type) {
    if (strcmp(type_str, "LOGIN") == 0) return LOGIN;
    if (strcmp(type_str, "LO_ACK") == 0) return LO_ACK;
    if (strcmp(type_str, "LO_NAK") == 0) return LO_NAK;
    if (strcmp(type_str, "EXIT") == 0) return EXIT;
    if (strcmp(type_str, "JOIN") == 0) return JOIN;
    if (strcmp(type_str, "JN_ACK") == 0) return JN_ACK;
    if (strcmp(type_str, "JN_NAK") == 0) return JN_NAK;
    if (strcmp(type_str, "LEAVE_SESS") == 0) return LEAVE_SESS;
    if (strcmp(type_str, "NEW_SESS") == 0) return NEW_SESS;
    if (strcmp(type_str, "NS_ACK") == 0) return NS_ACK;
    if (strcmp(type_str, "MESSAGE") == 0) return MESSAGE;
    if (strcmp(type_str, "QUERY") == 0) return QUERY;
    if (strcmp(type_str, "QU_ACK") == 0) return QU_ACK;
    return INVALID;
}
*/
