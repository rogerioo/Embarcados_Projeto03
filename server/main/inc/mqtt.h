#ifndef MQTT_H
#define MQTT_H

#define TAG "MQTT"
#define student_id "170021751"

void mqtt_start();

void mqtt_send_message(char *topic, const char *message, int adjust_topic);

#endif
