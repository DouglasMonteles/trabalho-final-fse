#ifndef __FIRE_DETECTOR__
#define __FIRE_DETECTOR__

#define ANALOGIC_CHANEL ADC2_CHANNEL_0
#define DIGITAL_CHANEL ADC2_CHANNEL_3

#define GPIO_ANALOGIC_OUTPUT  15
#define GPIO_DIGITAL_OUTPUT   4

void init_fire_detector();
void config_fire_detector();
void handle_fire_detector(void * params);
void mqtt_send_message_to_dashboard_about_flame_detector(int state);

#endif