#ifndef NVS_H
#define NVS_H

void start_nvs();
void write_value(const char *variable_name, const char *value);
const char *read_value(const char *variable_name);

#endif