#include "app_config.h"
#include "main.h"
#include "timeout.h"

#define	GET_TICK()	HAL_GetTick()

static uint32_t timeout_get_tick(void)
{
    return (GET_TICK());
}

/**
 * @brief timeout function
 * @param   timer   timer instance identifier
 * @param   timeout timeout in ms
 * 
 * @return  none
*/
void TimeoutSet(TIMER *timer, uint32_t timeout)
{   
    timer->TimeOutVal = timeout;
    timer->TickValCache = GET_TICK();    //timeout_get_tick();
}


/**
 * @brief   Is timer timeout
 * @param   timer timer instance
 * @return  1: timeout
 *          0: no timeout (running)
*/
bool IsTimeout(TIMER *timer)
{
    if (GET_TICK() >= (timer->TimeOutVal + timer->TickValCache) )
    {
        return 1;
    }

    return 0;
}