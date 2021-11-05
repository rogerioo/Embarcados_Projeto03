#include <cJSON.h>

#include <message.h>

char const *message_to_json(message params)
{
    cJSON *request = cJSON_CreateObject();

    cJSON_AddStringToObject(request, "command", params.command);
    cJSON_AddStringToObject(request, "data", params.data);

    char *json = cJSON_Print(request);

    return json;
}

message json_to_message(char const *json)
{
    cJSON *response = cJSON_Parse(json);
    message _message;

    _message.command = cJSON_GetObjectItem(response, "command")->valuestring;
    _message.data = cJSON_GetObjectItem(response, "data")->valuestring;

    return _message;
}