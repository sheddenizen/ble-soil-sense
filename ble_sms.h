/* Copyright (c) 2013 Nordic Semiconductor. All Rights Reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the license.txt file.
 */

#ifndef BLE_SMS_H__
#define BLE_SMS_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"

//#define SMS_UUID_BASE {0x23, 0xD1, 0xBC, 0xEA, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}
#define SMS_UUID_BASE {0x23, 0xa9, 0x1f, 0xb8, 0x0d, 0xeb, 0x4a, 0xcc, 0x9d, 0xf1, 0x43, 0x98, 0x0, 0x0, 0x50, 0x08}
#define SMS_UUID_SERVICE 0xc998
#define SMS_UUID_SOIL_MOISTURE_CHAR 0xc999

// Soil moisture characteristic type
typedef uint16_t soil_moisture_t;

// Forward declaration of the ble_sms_t type.
typedef struct ble_sms_s ble_sms_t;

typedef void (*ble_sms_led_write_handler_t) (ble_sms_t * p_sms, uint8_t new_state);

typedef struct
{
} ble_sms_init_t;

/**@brief LED Button Service structure. This contains various status information for the service. */
typedef struct ble_sms_s
{
    uint16_t                    service_handle;
    ble_gatts_char_handles_t    led_char_handles;
    ble_gatts_char_handles_t    button_char_handles;
    uint8_t                     uuid_type;
    uint16_t                    conn_handle;
} ble_sms_t;

/**@brief Function for initializing the LED Button Service.
 *
 * @param[out]  p_sms       LED Button Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_sms_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_sms_init(ble_sms_t * p_sms, const ble_sms_init_t * p_sms_init);

/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the LED Button Service.
 *
 *
 * @param[in]   p_sms      LED Button Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_sms_on_ble_evt(ble_sms_t * p_sms, ble_evt_t * p_ble_evt);

/**@brief Function for sending a button state notification.
 */
uint32_t ble_sms_on_moisture_update(ble_sms_t * p_sms, soil_moisture_t soil_moisture);

#endif // BLE_SMS_H__

/** @} */
