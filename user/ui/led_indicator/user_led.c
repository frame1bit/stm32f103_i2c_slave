/**
 * just for led test purpose 
 * 
*/
#include "main.h"
#include "app_config.h"
#include "utility/timeout.h"

#define USER_LED_PORT   LED_R_GPIO_Port
#define USER_LED_PIN    LED_R_Pin

TIMER timer_user_led;

#define LED_BLINK_MODE_SYS_CYCLE        1
#define LED_BLINK_MODE_BLINK_TICK       2
#define LED_BLINK_MODE_BLINK_PERIODE    3

#define BLINK_MODE  (LED_BLINK_MODE_BLINK_PERIODE)

#if (BLINK_MODE==LED_BLINK_MODE_BLINK_TICK)
#define MULTIPLIER      (5)
#define T_ON            (MULTIPLIER*100)
#define T_OFF           (MULTIPLIER*900)
#define T_ON_OFF        (T_ON+T_OFF)
static uint32_t count_led = 0;

#endif

void user_led_init(void)
{
    TimeoutSet(&timer_user_led, 500);
}


void user_led_run(void)
{

#if (BLINK_MODE==LED_BLINK_MODE_BLINK_PERIODE)
    if (IsTimeout(&timer_user_led) ) {
        // toggle led on PA0
        HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
        TimeoutSet(&timer_user_led, 500);
    }
#elif (BLINK_MODE==LED_BLINK_MODE_BLINK_TICK)
    if (count_led < T_ON) {
        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_RESET);
    } 
    else if (count_led < T_OFF) {
        HAL_GPIO_WritePin(USER_LED_GPIO_Port, USER_LED_Pin, GPIO_PIN_SET);
    }
    count_led = (count_led + 1) % T_ON_OFF;

#elif (BLINK_MODE==LED_BLINK_MODE_SYS_CYCLE)
    HAL_GPIO_TogglePin(USER_LED_GPIO_Port, USER_LED_Pin);
#endif

}
