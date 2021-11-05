#ifndef MESSAGE_H
#define MESSAGE_H

struct message
{
    const char *command;
    const char *data;
} typedef message;

char const *message_to_json(message params);
message json_to_message(char const *json);

#endif