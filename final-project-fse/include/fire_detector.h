#ifndef __FIRE_DETECTOR__
#define __FIRE_DETECTOR__

#define ANALOGIC_CHANEL ADC1_CHANNEL_7
#define DIGITAL_CHANEL ADC2_CHANNEL_4

#define GPIO_ANALOGIC_OUTPUT  35
#define GPIO_DIGITAL_OUTPUT   13

void init_fire_detector();
void config_fire_detector();
void handle_fire_detector(void * params);

#endif