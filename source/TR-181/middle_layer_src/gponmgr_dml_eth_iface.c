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
#include "gponmgr_dml_eth_iface.h"
#include "gponmgr_dml_obj.h"

#define MAX_STR_ARR_SIZE 256
#define STR_TRUE "true"
#define STR_FALSE "false"

static ANSC_STATUS Gponmgr_eth_getParams(char *pComponent, char *pBus, char *pParamName, char *pReturnVal)
{
    CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;
    parameterValStruct_t **retVal;
    char *ParamName[1];
    int ret = 0,
        nval;

    //Assign address for get parameter name
    ParamName[0] = pParamName;

    ret = CcspBaseIf_getParameterValues(
        bus_handle,
        pComponent,
        pBus,
        ParamName,
        1,
        &nval,
        &retVal);

    //Copy the value
    if (CCSP_SUCCESS == ret)
    {
        CcspTraceWarning(("%s parameterValue[%s]\n", __FUNCTION__, retVal[0]->parameterValue));

        if (NULL != retVal[0]->parameterValue)
        {
            memcpy(pReturnVal, retVal[0]->parameterValue, strlen(retVal[0]->parameterValue) + 1);
        }

        if (retVal)
        {
            free_parameterValStruct_t(bus_handle, nval, retVal);
        }

        return ANSC_STATUS_SUCCESS;
    }

    if (retVal)
    {
        free_parameterValStruct_t(bus_handle, nval, retVal);
    }

    return ANSC_STATUS_FAILURE;
}

ANSC_STATUS Gponmgr_eth_setParams( char *pComponent, char *pBus, char *pParamName,
                                          char *pParamVal, enum dataType_e type,
                                          unsigned int bCommitFlag)
{
    CCSP_MESSAGE_BUS_INFO *bus_info = (CCSP_MESSAGE_BUS_INFO *)bus_handle;
    parameterValStruct_t param_val[1] = {0};
    char *faultParam = NULL;
    int ret = 0;

    if( (pComponent == NULL) || (pBus == NULL) || (pParamName == NULL) || (pParamVal == NULL))
    {
        CcspTraceError(("%s %d - Invalid Input\n", __FUNCTION__,__LINE__));
        return ANSC_STATUS_FAILURE;

    }

    param_val[0].parameterName = pParamName;

    param_val[0].parameterValue = pParamVal;

    param_val[0].type = type;

    ret = CcspBaseIf_setParameterValues(
        bus_handle,
        pComponent,
        pBus,
        0,
        0,
        &param_val,
        1,
        bCommitFlag,
        &faultParam);

    if ((ret != CCSP_SUCCESS) && (faultParam != NULL))
    {
        CcspTraceError(("%s-%d Failed to set %s\n", __FUNCTION__, __LINE__, pParamName));
        bus_info->freefunc(faultParam);
        return ANSC_STATUS_FAILURE;
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGetLowerLayersInstanceInWanManager(char *pLowerLayers, INT *piInstanceNumber)
{
    char acTmpReturnValue[256] = {0};
    INT iLoopCount = 0;
    INT iTotalNoofEntries = 0;

    if (ANSC_STATUS_FAILURE == Gponmgr_eth_getParams(WAN_MGR_COMPONENT_NAME, WAN_MGR_DBUS_PATH, WAN_NOE_PARAM_NAME, acTmpReturnValue))
    {
        CcspTraceError(("%s %d Failed to get param value\n", __FUNCTION__, __LINE__));
        return ANSC_STATUS_FAILURE;
    }

    //Total count
    iTotalNoofEntries = atoi(acTmpReturnValue);
    CcspTraceInfo(("%s %d - TotalNoofEntries:%d\n", __FUNCTION__, __LINE__, iTotalNoofEntries));

    if (0 >= iTotalNoofEntries)
    {
        return ANSC_STATUS_SUCCESS;
    }

    //Traverse from loop
    for (iLoopCount = 0; iLoopCount < iTotalNoofEntries; iLoopCount++)
    {
        char acTmpQueryParam[256] = {0};

        //Query
        snprintf(acTmpQueryParam, sizeof(acTmpQueryParam), WAN_IF_NAME_PARAM_NAME, iLoopCount + 1);

        memset(acTmpReturnValue, 0, sizeof(acTmpReturnValue));
        if (ANSC_STATUS_FAILURE == Gponmgr_eth_getParams(WAN_MGR_COMPONENT_NAME, WAN_MGR_DBUS_PATH, acTmpQueryParam, acTmpReturnValue))
        {
            CcspTraceError(("%s %d Failed to get param value\n", __FUNCTION__, __LINE__));
            continue;
        }

        //Compare name
        if (0 == strcmp(acTmpReturnValue, pLowerLayers))
        {
            *piInstanceNumber = iLoopCount + 1;
            break;
        }
    }

    return ANSC_STATUS_SUCCESS;
}

ANSC_STATUS CosaDmlGponSetPhyStatusForWanManager(int iVeipIndex ,char *LowerLayers, char *PhyStatus)
{
    char ParamName[256] = {0};
    char ParamVal[256] = {0};
    char ifname[16] = {0};
    PDML_VEIP pGponVeip = NULL;
    INT iLinkInstance = -1;
    INT iWANInstance = -1;

    if (PhyStatus == NULL)
    {
        CcspTraceError(("%s Invalid Physical Status\n", __FUNCTION__));
        return ANSC_STATUS_FAILURE;
    }

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        DML_VEIP_CTRL_T* pGponVeipCtrl = pGponDmlData->gpon.Veip.pdata[iVeipIndex];
        if(pGponVeipCtrl!= NULL)
        {
            pGponVeip = &(pGponVeipCtrl->dml);

            // get veip interface structure
            if (pGponVeip == NULL)
            {
                CcspTraceError(("Error: Null Veip Interface\n"));
                GponMgrDml_GetData_release(pGponDmlData);
                return ANSC_STATUS_FAILURE;
            }

            //get Veip InterDomain Name
            snprintf(ifname, MAX_STR_ARR_SIZE, "%s", pGponVeip->InterfaceName);
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    //Get Instance for corresponding name
    if (ANSC_STATUS_FAILURE == CosaDmlGetLowerLayersInstanceInWanManager(ifname, &iWANInstance))
    {
        CcspTraceError(("%s %d Failed to get Instance value\n", __FUNCTION__, __LINE__));
        return ANSC_STATUS_FAILURE;
    }

    if (iWANInstance == -1)
    {
        CcspTraceError(("%s %d WAN instance not present\n", __FUNCTION__, __LINE__));
        return ANSC_STATUS_FAILURE;
    }

    CcspTraceInfo(("%s %d WAN Instance:%d\n", __FUNCTION__, __LINE__, iWANInstance));

    memset(ParamName, 0, sizeof(ParamName));
    snprintf(ParamName, sizeof(ParamName),WAN_BASE_INTERFACE_PARAM_NAME, iWANInstance);
    if (ANSC_STATUS_FAILURE == Gponmgr_eth_getParams(WAN_MGR_COMPONENT_NAME, WAN_MGR_DBUS_PATH,ParamName,ParamVal))
    {
        CcspTraceError(("%s %d Failed to get param value\n", __FUNCTION__, __LINE__));
        return ANSC_STATUS_FAILURE;
    }

    if (strncmp(ParamVal, LowerLayers,sizeof(ParamVal)) != 0)
    {
        CcspTraceError(("%s %d BaseInterface is not matching with LowerLayer\n", __FUNCTION__, __LINE__));
        return ANSC_STATUS_FAILURE;      
    }
	
    //Set Link Status
    memset(ParamName, 0, sizeof(ParamName));
    snprintf(ParamName, sizeof(ParamName),WAN_LINK_STATUS_PARAM_NAME, iWANInstance);

    if (Gponmgr_eth_setParams(WAN_MGR_COMPONENT_NAME, WAN_MGR_DBUS_PATH, ParamName, PhyStatus, ccsp_string, TRUE) != ANSC_STATUS_SUCCESS)
    {
        CcspTraceError(("%s %d: Unable to set param name %s\n", __FUNCTION__, __LINE__,ParamName));
        return ANSC_STATUS_FAILURE;
    }
 
    CcspTraceInfo(("%s %d Successfully notified %s event to WAN Manager for %s interface\n", __FUNCTION__, __LINE__, PhyStatus, ifname));

    return ANSC_STATUS_SUCCESS;

}
