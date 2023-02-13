#ifndef __LED__
#define __LED__

#define LED 2
#define LOW 0
#define HIGH 1
#define LED_TAG "LED"
#define LED_STATUS_METHOD "led_status"

void handle_led_event(char* event_data);
void config_led();
void turn_led_on_or_off();

#endif