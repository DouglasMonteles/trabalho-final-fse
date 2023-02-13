#ifndef __MAIN__
#define __MAIN__

#include "wifi.h"

#define LOW_POWER_MODE CONFIG_LOW_POWER_MODE
#define LOW_POWER 1

void mqtt_send_low_power_mode(int is_low_power_enabled);

#endif