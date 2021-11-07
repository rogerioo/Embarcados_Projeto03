#ifndef NVS_H
#define NVS_H

void start_nvs();
void nvs_write_value(const char *variable_name, const char *value);
const char *nvs_read_value(const char *variable_name);
void clean_nvs_partition();

#endif