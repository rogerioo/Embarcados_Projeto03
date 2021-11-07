#ifndef BUTTON_H
#define BUTTON_H

#define BUTTON_PIN 0

void button_init();
void IRAM_ATTR gpio_isr_handler(void *args);

#endif