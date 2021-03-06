/*
 * Copyright (c) 2015 ARM Limited. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "include/nsdlaccesshelper.h"
#include "include/m2mnsdlinterface.h"

M2MNsdlInterfaceList __nsdl_interface_list;

// callback function for NSDL library to call into

M2MConnectionHandler *__connection_handler = NULL;


uint8_t __nsdl_c_callback(struct nsdl_s *nsdl_handle,
                          sn_coap_hdr_s *received_coap_ptr,
                          sn_nsdl_addr_s *address,
                          sn_nsdl_capab_e nsdl_capab)
{
    uint8_t status = 0;
    M2MNsdlInterface  *interface = get_interface(nsdl_handle);
    if(interface) {
        status = interface->resource_callback(nsdl_handle,received_coap_ptr,
                                                     address, nsdl_capab);
    }
    return status;
}

void* __nsdl_c_memory_alloc(uint16_t size)
{
    if(size)
        return malloc(size);
    else
        return 0;
}

void __nsdl_c_memory_free(void *ptr)
{
    if(ptr)
        free(ptr);
}

uint8_t __nsdl_c_send_to_server(struct nsdl_s * nsdl_handle,
                                sn_nsdl_capab_e protocol,
                                uint8_t *data_ptr,
                                uint16_t data_len,
                                sn_nsdl_addr_s *address_ptr)
{
    uint8_t status = 0;
    M2MNsdlInterface  *interface = get_interface(nsdl_handle);
    if(interface) {
        status = interface->send_to_server_callback(nsdl_handle,
                                                           protocol, data_ptr,
                                                           data_len, address_ptr);
    }
    return status;
}

uint8_t __nsdl_c_received_from_server(struct nsdl_s * nsdl_handle,
                                      sn_coap_hdr_s *coap_header,
                                      sn_nsdl_addr_s *address_ptr)
{
    uint8_t status = 0;
    M2MNsdlInterface  *interface = get_interface(nsdl_handle);
    if(interface) {
        status = interface->received_from_server_callback(nsdl_handle,
                                                                 coap_header,
                                                                 address_ptr);
    }
    return status;
}

void __nsdl_c_bootstrap_done(sn_nsdl_oma_server_info_t *server_info_ptr, struct nsdl_s * nsdl_handle)
{
#ifndef YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
    M2MNsdlInterface  *interface = get_interface(nsdl_handle);
    if(interface) {
        interface->bootstrap_done_callback(server_info_ptr);
    }
#endif //YOTTA_CFG_DISABLE_BOOTSTRAP_FEATURE
}

void* __socket_malloc( void * context, size_t size)
{
    (void) context;
    return malloc(size);
}

void __socket_free(void * context, void * ptr)
{
    (void) context;
    free(ptr);
}

M2MNsdlInterface* get_interface(struct nsdl_s* nsdl_handle)
{
    M2MNsdlInterfaceList::const_iterator it;
    it = __nsdl_interface_list.begin();
    M2MNsdlInterface* obj = NULL;
    if (nsdl_handle) {
        for (; it!=__nsdl_interface_list.end(); it++) {
            if ((*it)->get_nsdl_handle() == nsdl_handle) {
                obj = *it;
                break;
            }
        }
    }
    return obj;
}

void __mutex_claim()
{
    if(__connection_handler) {
        __connection_handler->claim_mutex();
    }
}

void __mutex_release()
{
    if(__connection_handler) {
        __connection_handler->release_mutex();
    }
}
