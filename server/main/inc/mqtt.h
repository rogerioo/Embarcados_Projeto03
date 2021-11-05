#ifndef MQTT_H
#define MQTT_H

void mqtt_start();

void mqtt_send_message(char *topic, char *message, int adjust_topic);

#endif
