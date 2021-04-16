/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2019 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

/**********************************************************************
   Copyright [2014] [Cisco Systems, Inc.]

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
**********************************************************************/

#ifndef __GPONMGR_DML_ETH_IFACE_H__
#define __GPONMGR_DML_ETH_IFACE_H__


#include <ccsp_message_bus.h>
#include "gpon_apis.h"
#include "gponmgr_dml_obj.h"
#include "gponmgr_dml_func.h"
#include "gponmgr_dml_backendmgr.h"
#include "gponmgr_dml_plugin_main.h"

//WAN Agent
#define WAN_MGR_DBUS_PATH                     "/com/cisco/spvtg/ccsp/wanmanager"
#define WAN_MGR_COMPONENT_NAME                "eRT.com.cisco.spvtg.ccsp.wanmanager"
#define WAN_MGR_GPON_CPE_NAME                 "Device.X_RDK_WanManager.CPEInterface.3.Name"

//ETH Manager
#define ETH_MGR_COMPONENT_NAME                "eRT.com.cisco.spvtg.ccsp.ethagent"
#define ETH_MGR_DBUS_PATH                     "/com/cisco/spvtg/ccsp/ethagent"

#define ETH_MGR_IFACE_TABLE_NAME              "Device.Ethernet.X_RDK_Interface."
#define ETH_MGR_IFACE                         "Device.Ethernet.X_RDK_Interface.%d."
#define ETH_MGR_NO_OF_IFACE                   "Device.Ethernet.X_RDK_InterfaceNumberOfEntries"
#define ETH_MGR_IFACE_NAME                    ETH_MGR_IFACE"Name"
#define ETH_MGR_IFACE_LOWERLAYERS             ETH_MGR_IFACE"LowerLayers"
#define ETH_MGR_IFACE_ENABLE                  ETH_MGR_IFACE"Enable"


extern ANSC_HANDLE bus_handle;

ANSC_STATUS Gponmgr_eth_addInterface( int iVeipIndex, char *LowerLayers, int *iVeipInstance);

ANSC_STATUS Gponmgr_eth_setEnableInterface(int iVeipInstance, BOOL bflag);

ANSC_STATUS Gponmgr_eth_setParams( char *pComponent, char *pBus, char *pParamName, char *pParamVal, enum dataType_e type, unsigned int bCommitFlag);

#endif
