/*
 * debug.h
 *
 *  Created on: 13 Feb 2018
 *      Author: dan
 */

#ifndef DEBUG_H_
#define DEBUG_H_


//#include "nrf.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_gpio.h"
#include "boards.h"


#ifndef NRF_LOG_MODULE_NAME
#pragma error "Module not set in source file"
#endif

#define DEBUG_LED_PIN					BSP_LED_3	/* To work out WTF is going on */

#define APP_CHECK_RESULT(x) \
	if (x == NRF_SUCCESS)	\
	{		\
		NRF_LOG_INFO("OK: %s:%u, %s Result = 0x%x\r\n", (uint32_t)__FILE__, __LINE__, (uint32_t)__func__, (uint32_t)x );	\
	}		\
	else	\
	{		\
		NRF_LOG_ERROR("ERR: %s:%u %s Result = 0x%x\r\n", (uint32_t)__FILE__, __LINE__, (uint32_t)__func__, (uint32_t)x );	\
	}

#define APP_CHECK_RES_C(x, c) \
	if (x == NRF_SUCCESS)	\
	{		\
		NRF_LOG_INFO("OK: %s:%u, in %s %s Result = 0x%x\r\n", (uint32_t)__FILE__, __LINE__, (uint32_t)__func__, (uint32_t)c, (uint32_t)x );	\
	}		\
	else	\
	{		\
		NRF_LOG_ERROR("ERR: %s:%u, in %s %s Result = 0x%x\r\n", (uint32_t)__FILE__, __LINE__, (uint32_t)__func__, (uint32_t)c, (uint32_t)x );	\
	}

typedef enum {
	DBG_OFF,
	DBG_ON,
	DBG_TOGGLE
} dbg_state_t;

static inline void led_debug(dbg_state_t dbgState)
{
	extern uint8_t led_debug_last_state;
	uint8_t newState = (dbgState == DBG_TOGGLE) ? !led_debug_last_state : (uint8_t)dbgState;

    if (newState)
    {
        nrf_gpio_pin_set(DEBUG_LED_PIN);
    }
    else
    {
        nrf_gpio_pin_clear(DEBUG_LED_PIN);
    }
    led_debug_last_state = newState;
}



#endif /* DEBUG_H_ */
