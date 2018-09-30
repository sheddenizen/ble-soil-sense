/* Copyright (c) 2018 Daniel Steer.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the license.txt file.
 */

#include "ble_sms.h"
#include <string.h>
#include "nordic_common.h"
#include "ble_srv_common.h"
#include "app_util.h"
#include "ble_gatts.h"
#include "nrf_error.h"

#define NRF_LOG_MODULE_NAME "SMS"
#include "debug.h"

/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_sms       LED Button Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_sms_t * p_sms, ble_evt_t * p_ble_evt)
{
    p_sms->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_sms       LED Button Service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_sms_t * p_sms, ble_evt_t * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_sms->conn_handle = BLE_CONN_HANDLE_INVALID;
}


void ble_sms_on_ble_evt(ble_sms_t * p_sms, ble_evt_t * p_ble_evt)
{
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            NRF_LOG_INFO("ble_sms_on_ble_evt: BLE_GAP_EVT_CONNECTED\r\n");
            on_connect(p_sms, p_ble_evt);
            break;
            
        case BLE_GAP_EVT_DISCONNECTED:
            NRF_LOG_INFO("ble_sms_on_ble_evt: BLE_GAP_EVT_DISCONNECTED\r\n");
            on_disconnect(p_sms, p_ble_evt);
            break;
            
        default:
            // No implementation needed.
            break;
    }
}


/**@brief Function for adding the Button characteristic.
 *
 */
static uint32_t soil_moisture_char_add(ble_sms_t * p_sms, const ble_sms_init_t * p_sms_init)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_md_t cccd_md;
    ble_gatts_attr_t    attr_char_value;
    ble_uuid_t          ble_uuid;
    ble_gatts_attr_md_t attr_md;

    memset(&cccd_md, 0, sizeof(cccd_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc = BLE_GATTS_VLOC_STACK;
    
    memset(&char_md, 0, sizeof(char_md));
    
    char_md.char_props.read   = 1;
    char_md.char_props.notify = 1;
    char_md.p_char_user_desc  = NULL;
    char_md.p_char_pf         = NULL;
    char_md.p_user_desc_md    = NULL;
    char_md.p_cccd_md         = &cccd_md;
    char_md.p_sccd_md         = NULL;
    
    ble_uuid.type = p_sms->uuid_type;
    ble_uuid.uuid = SMS_UUID_SOIL_MOISTURE_CHAR;
    
    memset(&attr_md, 0, sizeof(attr_md));

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_NO_ACCESS(&attr_md.write_perm);
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 0;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 0;
    
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = sizeof(soil_moisture_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = sizeof(soil_moisture_t);
    attr_char_value.p_value      = NULL;
    
    uint32_t err_code = sd_ble_gatts_characteristic_add(p_sms->service_handle, &char_md,
                                               &attr_char_value,
                                               &p_sms->button_char_handles);
    APP_CHECK_RES_C(err_code, "sd_ble_gatts_characteristic_add");
    return err_code;
}

uint32_t ble_sms_init(ble_sms_t * p_sms, const ble_sms_init_t * p_sms_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_sms->conn_handle       = BLE_CONN_HANDLE_INVALID;
    
    // Add service
    ble_uuid128_t base_uuid = {SMS_UUID_BASE};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_sms->uuid_type);
    APP_CHECK_RES_C(err_code, "sd_ble_uuid_vs_add");
    
    ble_uuid.type = p_sms->uuid_type;
    ble_uuid.uuid = SMS_UUID_SERVICE;

    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_sms->service_handle);
    APP_CHECK_RES_C(err_code, "sd_ble_gatts_service_add");
    
    err_code = soil_moisture_char_add(p_sms, p_sms_init);
    APP_CHECK_RES_C(err_code, "soil_moisture_char_add");
    
    return NRF_SUCCESS;
}

uint32_t ble_sms_on_moisture_update(ble_sms_t * p_sms, uint16_t soil_moisture)
{
    ble_gatts_hvx_params_t params;
    uint16_t len = sizeof(soil_moisture);
    
    memset(&params, 0, sizeof(params));
    params.type = BLE_GATT_HVX_NOTIFICATION;
    params.handle = p_sms->button_char_handles.value_handle;
    params.p_data = (uint8_t *)&soil_moisture;
    params.p_len = &len;
    
    uint32_t err_code = sd_ble_gatts_hvx(p_sms->conn_handle, &params);
    APP_CHECK_RES_C(err_code, "sd_ble_gatts_hvx");
    return err_code;
}
