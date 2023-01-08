#include "main.h"

BleGamepad bleGamepad("BlueN64 Gamepad", "JPZV");
BleGamepadConfiguration bleGamepadConfig;

TaskHandle_t loopTaskHandle = NULL;

#define buttonsLength 9

n64Button buttons[buttonsLength] =
{
    {BUTTON_A_PIN,      BUTTON_1},
    {BUTTON_B_PIN,      BUTTON_2},
    {TRIGGER_L_PIN,     BUTTON_7},
    {TRIGGER_R_PIN,     BUTTON_8},
    {C_DOWN_PIN,        BUTTON_5},
    {C_LEFT_PIN,        BUTTON_4},
    {TRIGGER_Z_PIN,     BUTTON_9},
    {TRIGGER_Z_PIN,     BUTTON_10},
    {BUTTON_START_PIN,  BUTTON_6},
};
n64Button startButton = 
{
    BUTTON_START_PIN,  START_BUTTON
};

int btn_x_axis = 0;
int btn_y_axis = 0;
int dpad_axis = 0;
int x_axis = 0;
int y_axis = 0;

int prev_x_axis = 0;
int prev_y_axis = 0;
int prev_c_x_axis = 0;
int prev_c_y_axis = 0;
int prev_dpad_axis = 0;
bool is_button_pressed = false;
bool need_report = false;
void app_loop(void *params)
{
    while (true)
    {
        if (bleGamepad.isConnected())
        {
            btn_x_axis = btn_y_axis = 0;
            need_report = false;
            for (int i = 0; i < buttonsLength; i++)
            {
                if ((is_button_pressed = !gpio_get_level(buttons[i].pinNumber)))
                    bleGamepad.press(buttons[i].button);
                else
                    bleGamepad.release(buttons[i].button);

                if (buttons[i].state != is_button_pressed)
                    need_report = true;
                buttons[i].state = is_button_pressed;
            }

            if ((is_button_pressed = !gpio_get_level(startButton.pinNumber)))
                bleGamepad.pressSpecialButton(startButton.button);
            else
                bleGamepad.releaseSpecialButton(startButton.button);
            if (startButton.state != is_button_pressed)
                need_report = true;
            startButton.state = is_button_pressed;

            if (!gpio_get_level(DPAD_UP_PIN))
                btn_y_axis++;
            if (!gpio_get_level(DPAD_DOWN_PIN))
                btn_y_axis--;
            if (!gpio_get_level(DPAD_LEFT_PIN))
                btn_x_axis--;
            if (!gpio_get_level(DPAD_RIGHT_PIN))
                btn_x_axis++;
            
            if (btn_y_axis == 0 && btn_x_axis == 0)
            {
                dpad_axis = 0;
            }
            else
            {
                switch ((int)(atan2(btn_x_axis, btn_y_axis) * 180 / PI))
                {
                    default:
                    case 0:
                        dpad_axis = DPAD_UP;
                        break;
                    case 45:
                        dpad_axis = DPAD_UP_RIGHT;
                        break;
                    case 90:
                        dpad_axis = DPAD_RIGHT;
                        break;
                    case 135:
                        dpad_axis = DPAD_DOWN_RIGHT;
                        break;
                    case 180:
                        dpad_axis = DPAD_DOWN;
                        break;
                    case -135:
                        dpad_axis = DPAD_DOWN_LEFT;
                        break;
                    case -90:
                        dpad_axis = DPAD_LEFT;
                        break;
                    case -45:
                        dpad_axis = DPAD_UP_LEFT;
                        break;
                }
            }
            if (dpad_axis != prev_dpad_axis)
                need_report = true;
            prev_dpad_axis = dpad_axis;

            bleGamepad.setHat1(dpad_axis);

            btn_x_axis = btn_y_axis = 0;
            if (!gpio_get_level(C_DOWN_PIN))
                btn_y_axis = -1;
            if (!gpio_get_level(C_UP_PIN))
                btn_y_axis = 1;
            if (!gpio_get_level(C_LEFT_PIN))
                btn_x_axis = -1;
            if (!gpio_get_level(C_RIGHT_PIN))
                btn_x_axis = 1;

            bleGamepad.setAxes(((x_axis = n64_get_joystick_x()) + JOYSTICK_MAX_X) * JOYSTICK_ABS_MAX / JOYSTICK_MAX_X,
                               ((y_axis = -n64_get_joystick_y()) + JOYSTICK_MAX_Y) * JOYSTICK_ABS_MAX / JOYSTICK_MAX_Y,
                               0, 0,
                               (btn_x_axis + 1) * JOYSTICK_ABS_MAX,
                               (btn_y_axis + 1) * JOYSTICK_ABS_MAX);
            if (x_axis != prev_x_axis || y_axis != prev_y_axis || btn_x_axis != prev_c_x_axis || btn_y_axis != prev_c_y_axis)
                need_report = true;
            prev_x_axis = x_axis;
            prev_y_axis = y_axis;
            prev_c_x_axis = btn_x_axis;
            prev_c_y_axis = btn_y_axis;

            if (need_report)
                bleGamepad.sendReport();
        }
        blucontrol_handle_buttons();
    }
}

extern "C" void app_main(void)
{
    printf("BlueN64 Control Generic Mode. HEAP=%#010lx\n", esp_get_free_heap_size());

    blucontrol_mode_init();
    n64_init();

    bleGamepadConfig.setAutoReport(false);
    bleGamepadConfig.setButtonCount(10);
    bleGamepadConfig.setAxesMin(0x0000);
    bleGamepadConfig.setAxesMax(JOYSTICK_ABS_MAX * 2);
    bleGamepadConfig.setWhichSpecialButtons(true, false, false, false, false, false, false, false);
    bleGamepadConfig.setWhichAxes(true, true, false, true, true, false, false, false);
    bleGamepad.begin(&bleGamepadConfig);

    xTaskCreatePinnedToCore(app_loop, "APP_LOOP", 4096, NULL, tskIDLE_PRIORITY, &loopTaskHandle, 1);
    configASSERT(loopTaskHandle);

    printf("BlueN64 Control. Started!\n");
}