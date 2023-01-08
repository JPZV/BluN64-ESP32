#include "n64-pad.h"

int joystick_x_value = 0;
int joystick_y_value = 0;

void joystick_interrupt_handler(void *params)
{
    switch ((int)params)
    {
        default:
        case JOYSTICK_X_AXIS:
            if (gpio_get_level(JOYSTICK_X_INT_PIN) == gpio_get_level(JOYSTICK_X_Q_PIN))
            {
                joystick_x_value--;
            }
            else
            {
                joystick_x_value++;
            }

            //Capping X to -40 <= X <= 40
            if (joystick_x_value < 0 && joystick_x_value < -JOYSTICK_MAX_X)
            {
                joystick_x_value = -JOYSTICK_MAX_X;
            }
            else if (joystick_x_value > 0 && joystick_x_value > JOYSTICK_MAX_X)
            {
                joystick_x_value = JOYSTICK_MAX_X;   
            }
            break;
        case JOYSTICK_Y_AXIS:
            if (gpio_get_level(JOYSTICK_Y_INT_PIN) == gpio_get_level(JOYSTICK_Y_Q_PIN))
            {
                joystick_y_value--;
            }
            else
            {
                joystick_y_value++;
            }
            
            //Capping Y to -40 <= Y <= 40
            if (joystick_y_value < 0 && joystick_y_value < -JOYSTICK_MAX_Y)
            {
                joystick_y_value = -JOYSTICK_MAX_Y;
            }
            else if (joystick_y_value > 0 && joystick_y_value > JOYSTICK_MAX_Y)
            {
                joystick_y_value = JOYSTICK_MAX_Y;   
            }
            break;
    }
}

void n64_init(void)
{
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = BUTTONS_BIT_MASK;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = JOYSTICK_Q_BIT_MASK;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.pin_bit_mask = JOYSTICK_INT_BIT_MASK;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    ESP_ERROR_CHECK(gpio_install_isr_service(0));
    ESP_ERROR_CHECK(gpio_isr_handler_add(JOYSTICK_X_INT_PIN, joystick_interrupt_handler, (void *)JOYSTICK_X_AXIS));
    ESP_ERROR_CHECK(gpio_isr_handler_add(JOYSTICK_Y_INT_PIN, joystick_interrupt_handler, (void *)JOYSTICK_Y_AXIS));
}

int n64_get_joystick_x(void)
{
    return joystick_x_value;
}
int n64_get_joystick_y(void)
{
    return joystick_y_value;
}