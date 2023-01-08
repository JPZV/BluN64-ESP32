#ifndef MAIN_H
#define MAIN_H

#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include "NimBLEDevice.h"
#include <BleGamepad.h>
#include <tgmath.h>

#define PI 3.14159265

#ifdef __cplusplus
extern "C" {
#endif
#include "blucontrol_mode.h"
#include "n64-pad.h"
#ifdef __cplusplus
}
#endif

struct n64Button
{
    gpio_num_t pinNumber;
    int button;
    bool state = false;
};

#endif