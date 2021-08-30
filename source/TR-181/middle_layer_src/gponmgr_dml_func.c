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

#include "ansc_platform.h"
#include "gpon_apis.h"
#include "gponmgr_dml_func.h"
#include "gponmgr_dml_backendmgr.h"
#include "gponmgr_dml_plugin_main.h"
#include "gponmgr_dml_obj.h"
#include "gponmgr_dml_hal.h"

#if     CFG_USE_CCSP_SYSLOG
    #include <ccsp_syslog.h>
#endif




/***********************************************************************
 IMPORTANT NOTE:

 According to TR69 spec:
 On successful receipt of a SetParameterValues RPC, the CPE MUST apply
 the changes to all of the specified Parameters atomically. That is, either
 all of the value changes are applied together, or none of the changes are
 applied at all. In the latter case, the CPE MUST return a fault response
 indicating the reason for the failure to apply the changes.

 The CPE MUST NOT apply any of the specified changes without applying all
 of them.

 In order to set parameter values correctly, the back-end is required to
 hold the updated values until "Validate" and "Commit" are called. Only after
 all the "Validate" passed in different objects, the "Commit" will be called.
 Otherwise, "Rollback" will be called instead.

 The sequence in COSA Data Model will be:

 SetParamBoolValue/SetParamIntValue/SetParamUlongValue/SetParamStringValue
 -- Backup the updated values;

 if( Validate_XXX())
 {
     Commit_XXX();    -- Commit the update all together in the same object
 }
 else
 {
     Rollback_XXX();  -- Remove the update at backup;
 }

***********************************************************************/
/***********************************************************************

 APIs for Object:


***********************************************************************/

/**********************************************************************

    caller:     owner of this object

    prototype:

        BOOL GponPhy_Validate(ANSC_HANDLE hInsContext,char* pReturnParamName,ULONG* puLength);

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE hInsContext,
                The instance handle;

                char*                       pReturnParamName,
                The buffer (128 bytes) of parameter name if there's a validation.

                ULONG*                      puLength
                The output length of the param name.

    return:     TRUE if there's no validation.

**********************************************************************/

BOOL GponPhy_Validate (ANSC_HANDLE hInsContext,char* pReturnParamName,ULONG* puLength)
{
    return TRUE;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG Gpon_Commit(ANSC_HANDLE hInsContext);

    description:

        This function is called to finally commit all the update.

    argument:   ANSC_HANDLE hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/

ULONG GponPhy_Commit(ANSC_HANDLE hInsContext)
{
    ANSC_STATUS                returnStatus  = ANSC_STATUS_SUCCESS;
    return returnStatus;
}

/**********************************************************************

    caller:     owner of this object

    prototype:

        ULONG Gpon_Rollback(ANSC_HANDLE hInsContext);

    description:

        This function is called to roll back the update whenever there's a
        validation found.

    argument:   ANSC_HANDLE hInsContext,
                The instance handle;

    return:     The status of the operation.

**********************************************************************/

BOOL GponPhy_IsUpdated(ANSC_HANDLE hInsContext)
{
    ANSC_STATUS ret = ANSC_STATUS_SUCCESS;
    BOOL        bIsUpdated = FALSE;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        ret = GponHal_get_pm(&(pGponDmlData->gpon.PhysicalMedia));
        if(ret == ANSC_STATUS_SUCCESS)
        {
            bIsUpdated = TRUE;
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return bIsUpdated;
}

ULONG GponPhy_Synchronize(ANSC_HANDLE hInsContext)
{

    ANSC_STATUS            returnStatus   = ANSC_STATUS_SUCCESS;

    return returnStatus;
}

ULONG GponPhy_GetEntryCount(ANSC_HANDLE hInsContext)
{
    ULONG count = 0;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        count = pGponDmlData->gpon.PhysicalMedia.ulQuantity;

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return count;
}

ANSC_HANDLE GponPhy_GetEntry(ANSC_HANDLE hInsContext,ULONG nIndex,ULONG* pInsNumber)
{
    ANSC_HANDLE pDmlEntry = NULL;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        if(nIndex < pGponDmlData->gpon.PhysicalMedia.ulQuantity)
        {
            DML_PHY_MEDIA_CTRL_T* pGponData = pGponDmlData->gpon.PhysicalMedia.pdata[nIndex];
            if(pGponData != NULL)
            {
                *pInsNumber = nIndex + 1;
                pDmlEntry = (ANSC_HANDLE) pGponData;
            }
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return pDmlEntry;
}

ULONG GponPhy_Rollback(ANSC_HANDLE hInsContext)
{
    return 0;
}

ULONG GponPhy_GetParamStringValue(ANSC_HANDLE hInsContext,char* ParamName,char* pValue,ULONG* pUlSize )
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    ULONG ret = -1;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "ModuleVendor", TRUE))
            {
                AnscCopyString(pValue, pGponPhy->ModuleVendor);
                ret = 0;
            }
            else if( AnscEqualString(ParamName, "ModuleName", TRUE))
            {
                AnscCopyString(pValue, pGponPhy->ModuleName);
                ret = 0;
            }
            else if( AnscEqualString(ParamName, "ModuleVersion", TRUE))
            {
                AnscCopyString(pValue, pGponPhy->ModuleVersion);
                ret = 0;
            }
            else if( AnscEqualString(ParamName, "ModuleFirmwareVersion", TRUE))
            {
                AnscCopyString(pValue, pGponPhy->ModuleFirmwareVersion);
                ret = 0;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhy_GetParamUlongValue (ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Cage", TRUE))
            {
                *puLong = (ULONG) pGponPhy->Cage;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "PonMode", TRUE))
            {
                *puLong = (ULONG) pGponPhy->PonMode;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Connector", TRUE))
            {
                *puLong = (ULONG) pGponPhy->Connector;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "NominalBitRateDownstream", TRUE))
            {
                *puLong = (ULONG) pGponPhy->NominalBitRateDownstream;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "NominalBitRateUpstream", TRUE))
            {
                *puLong = (ULONG) pGponPhy->NominalBitRateUpstream;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Status", TRUE))
            {
                *puLong = (ULONG) pGponPhy->Status;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "RedundancyState", TRUE))
            {
                *puLong = (ULONG) pGponPhy->RedundancyState;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhyRxpwr_GetParamIntValue(ANSC_HANDLE hInsContext,char* ParamName,int* pInt)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "SignalLevel", TRUE))
            {
                *pInt = (int) pGponPhy->RxPower.SignalLevel;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "SignalLevelLowerThreshold", TRUE))
            {
                *pInt = (int) pGponPhy->RxPower.SignalLevelLowerThreshold;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "SignalLevelUpperThreshold", TRUE))
            {
                *pInt = (int) pGponPhy->RxPower.SignalLevelUpperThreshold;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhyRxpwr_SetParamIntValue(ANSC_HANDLE hInsContext,char* ParamName,int iValue)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    char strValue[JSON_MAX_VAL_ARR_SIZE]={0};
    char strName[JSON_MAX_STR_ARR_SIZE]={0};
    BOOL ret = FALSE;


    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "SignalLevelLowerThreshold", TRUE))
            {
                sprintf(strName, GPON_HAL_PM_RX_POWER_LOWER_THR, pGponPhy->uInstanceNumber);
                snprintf(strValue, JSON_MAX_VAL_ARR_SIZE, "%d", iValue);
                if (GponHal_setParam(strName,PARAM_INTEGER,strValue) == ANSC_STATUS_SUCCESS)
                {
                    pGponPhy->RxPower.SignalLevelLowerThreshold=iValue;
                    ret = TRUE;
                }
            }
            else if( AnscEqualString(ParamName, "SignalLevelUpperThreshold", TRUE))
            {
                sprintf(strName, GPON_HAL_PM_RX_POWER_UPPER_THR, pGponPhy->uInstanceNumber);
                snprintf(strValue, JSON_MAX_VAL_ARR_SIZE, "%d", iValue);
                if (GponHal_setParam(strName,PARAM_INTEGER,strValue) == ANSC_STATUS_SUCCESS)
                {
                    pGponPhy->RxPower.SignalLevelUpperThreshold=iValue;
                    ret = TRUE;
                }
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhyTxpwr_GetParamIntValue(ANSC_HANDLE hInsContext,char* ParamName,int* pInt)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "SignalLevel", TRUE))
            {
                *pInt = (int) pGponPhy->TxPower.SignalLevel;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "SignalLevelLowerThreshold", TRUE))
            {
                *pInt = (int) pGponPhy->TxPower.SignalLevelLowerThreshold;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "SignalLevelUpperThreshold", TRUE))
            {
                *pInt = (int) pGponPhy->TxPower.SignalLevelUpperThreshold;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhyTxpwr_SetParamIntValue(ANSC_HANDLE hInsContext,char* ParamName,int iValue)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    char strValue[JSON_MAX_VAL_ARR_SIZE]={0};
    char strName[JSON_MAX_STR_ARR_SIZE]={0};
    BOOL ret = FALSE;


    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "SignalLevelLowerThreshold", TRUE))
            {
                sprintf(strName, GPON_HAL_PM_TX_POWER_LOWER_THR, pGponPhy->uInstanceNumber);
                snprintf(strValue, JSON_MAX_VAL_ARR_SIZE, "%d", iValue);
                if (GponHal_setParam(strName, PARAM_INTEGER, strValue) == ANSC_STATUS_SUCCESS)
                {
                    pGponPhy->TxPower.SignalLevelLowerThreshold=iValue;
                    ret = TRUE;
                }
            }
            else if( AnscEqualString(ParamName, "SignalLevelUpperThreshold", TRUE))
            {
                sprintf(strName, GPON_HAL_PM_TX_POWER_UPPER_THR, pGponPhy->uInstanceNumber);
                snprintf(strValue, JSON_MAX_VAL_ARR_SIZE, "%d", iValue);
                if (GponHal_setParam(strName, PARAM_INTEGER, strValue) == ANSC_STATUS_SUCCESS)
                {
                    pGponPhy->TxPower.SignalLevelUpperThreshold=iValue;
                    ret = TRUE;
                }
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhyVoltage_GetParamIntValue(ANSC_HANDLE hInsContext,char* ParamName,int* pInt)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "VoltageLevel", TRUE))
            {
                *pInt = (int) pGponPhy->Voltage.VoltageLevel;
                ret = TRUE;
            }
            
            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhyBias_GetParamUlongValue( ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "CurrentBias", TRUE))
            {
                *puLong = pGponPhy->Bias.CurrentBias;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhyTemperature_GetParamIntValue(ANSC_HANDLE hInsContext,char* ParamName,int* pInt)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "CurrentTemp", TRUE))
            {
                *pInt = (int) pGponPhy->Temperature.CurrentTemp;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhyPerformanceThreshold_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_PHY_MEDIA* pGponPhy = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "SignalFail", TRUE))
            {
                *puLong= pGponPhy->PerformThreshold.SignalFail;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "SignalDegrade", TRUE))
            {
                *puLong= pGponPhy->PerformThreshold.SignalDegrade;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponPhyAlarm_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_PHY_MEDIA_CTRL_T* pGponCtrl = (DML_PHY_MEDIA_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_ALARM* pGponPhyAlarm = &(pGponCtrl->dml.Alarm);

            if( AnscEqualString(ParamName, "RDI", TRUE))
            {
                *puLong = pGponPhyAlarm->RDI;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "PEE", TRUE))
            {
                *puLong =pGponPhyAlarm->PEE;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "LOS", TRUE))
            {
                *puLong = pGponPhyAlarm->LOS;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "LOF", TRUE))
            {
                *puLong = pGponPhyAlarm->LOF;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "DACT", TRUE))
            {
                *puLong = pGponPhyAlarm->DACT;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "DIS", TRUE))
            {
                *puLong = pGponPhyAlarm->DIS;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "MIS", TRUE))
            {
                *puLong = pGponPhyAlarm->MIS;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "MEM", TRUE))
            {
                *puLong = pGponPhyAlarm->MEM;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "SUF", TRUE))
            {
                *puLong = pGponPhyAlarm->SUF;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "SF", TRUE))
            {
                *puLong = pGponPhyAlarm->SF;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "SD", TRUE))
            {
                *puLong = pGponPhyAlarm->SD;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "LCDG", TRUE))
            {
                *puLong = pGponPhyAlarm->LCDG;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "TF", TRUE))
            {
                *puLong = pGponPhyAlarm->TF;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "ROGUE", TRUE))
            {
                *puLong = pGponPhyAlarm->ROGUE;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponGtc_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    BOOL ret = FALSE;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        DML_GTC* pGponGtc = &(pGponDmlData->gpon.Gtc);
        if(GponHal_get_gtc(pGponGtc) == ANSC_STATUS_SUCCESS)
        {
            if( AnscEqualString(ParamName, "CorrectedFecBytes", TRUE))
            {
                *puLong= pGponGtc->CorrectedFecBytes;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "CorrectedFecCodeWords", TRUE))
            {
                *puLong= pGponGtc->CorrectedFecCodeWords;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "UnCorrectedFecCodeWords", TRUE))
            {
                *puLong= pGponGtc->UnCorrectedFecCodeWords;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "TotalFecCodeWords", TRUE))
            {
                *puLong= pGponGtc->TotalFecCodeWords;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "HecErrorCount", TRUE))
            {
                *puLong= pGponGtc->HecErrorCount;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "PSBdHecErrors", TRUE))
            {
                *puLong= pGponGtc->PSBdHecErrors;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "FrameHecErrors", TRUE))
            {
                *puLong= pGponGtc->FrameHecErrors;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "FramesLost", TRUE))
            {
                *puLong= pGponGtc->FramesLost;
                ret = TRUE;
            }
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return ret;
}


BOOL GponPloam_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    BOOL ret = FALSE;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        DML_PLOAM* pGponPloam = &(pGponDmlData->gpon.Ploam);

        if ( GponHal_get_ploam(pGponPloam) == ANSC_STATUS_SUCCESS)
        {
            if( AnscEqualString(ParamName, "OnuId", TRUE))
            {
                *puLong= pGponPloam->OnuId;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "RegistrationState", TRUE))
            {
                *puLong= pGponPloam->RegistrationState;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "ActivationCounter", TRUE))
            {
                *puLong= pGponPloam->ActivationCounter;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "TxMessageCount", TRUE))
            {
                *puLong= pGponPloam->TxMessageCount;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "RxMessageCount", TRUE))
            {
                *puLong= pGponPloam->RxMessageCount;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "MicErrors", TRUE))
            {
                *puLong= pGponPloam->MicErrors;
                ret = TRUE;
            }
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return ret;
}

ULONG GponPloam_GetParamStringValue(ANSC_HANDLE hInsContext,char* ParamName,char* pValue,ULONG* pUlSize)
{
    ULONG ret = -1;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        DML_PLOAM* pGponPloam = &(pGponDmlData->gpon.Ploam);

        if ( GponHal_get_ploam(pGponPloam) == ANSC_STATUS_SUCCESS)
        {
            if( AnscEqualString(ParamName, "SerialNumber", TRUE))
            {
                AnscCopyString(pValue, pGponPloam->SerialNumber);
                ret = 0;
            }
            else if( AnscEqualString(ParamName, "VendorId", TRUE))
            {
                AnscCopyString(pValue, pGponPloam->VendorId);
                ret = 0;
            }
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return ret;
}

BOOL GponPloamRegTmr_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    BOOL ret = FALSE;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        DML_PLOAM* pGponPloam = &(pGponDmlData->gpon.Ploam);

        if( AnscEqualString(ParamName, "TO1", TRUE))
        {
            *puLong= pGponPloam->RegistrationTimers.TO1;
            ret = TRUE;
        }
        else if( AnscEqualString(ParamName, "TO2", TRUE))
        {
            *puLong= pGponPloam->RegistrationTimers.TO2;
            ret = TRUE;
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return ret;
}

BOOL GponGem_IsUpdated(ANSC_HANDLE hInsContext)
{
    ANSC_STATUS ret = ANSC_STATUS_SUCCESS;
    BOOL        bIsUpdated = FALSE;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        ret = GponHal_get_gem(&(pGponDmlData->gpon.Gem));
        if(ret == ANSC_STATUS_SUCCESS)
        {
            bIsUpdated = TRUE;
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return bIsUpdated;
}

ULONG GponGem_Synchronize(ANSC_HANDLE hInsContext)
{
    ANSC_STATUS            returnStatus   = ANSC_STATUS_SUCCESS;
    return returnStatus;
}

ULONG GponGem_GetEntryCount(ANSC_HANDLE hInsContext)
{
    ULONG count = 0;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        count = pGponDmlData->gpon.Gem.ulQuantity;

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return count;
}

ANSC_HANDLE GponGem_GetEntry(ANSC_HANDLE hInsContext,ULONG nIndex,ULONG* pInsNumber)
{
    ANSC_HANDLE pDmlEntry = NULL;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        if(nIndex < pGponDmlData->gpon.Gem.ulQuantity)
        {
            DML_GEM_CTRL_T* pGponData = pGponDmlData->gpon.Gem.pdata[nIndex];
            if(pGponData != NULL)
            {
                *pInsNumber = nIndex + 1;
                pDmlEntry = (ANSC_HANDLE) pGponData;
            }
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return pDmlEntry;
}

BOOL GponGem_Validate(ANSC_HANDLE hInsContext,char* pReturnParamName,ULONG* puLength)
{
    return TRUE;
}

ULONG GponGem_Commit(ANSC_HANDLE hInsContext)
{
    return 0;
}

ULONG GponGem_Rollback(ANSC_HANDLE hInsContext)
{
    return 0;
}

BOOL GponGem_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_GEM_CTRL_T* pGponCtrl = (DML_GEM_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_GEM* pGponGem = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "PortId", TRUE))
            {
                *puLong= pGponGem->PortId;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "TrafficType", TRUE))
            {
                *puLong= pGponGem->TrafficType;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "TransmittedFrames", TRUE))
            {
                *puLong= pGponGem->TransmittedFrames;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "ReceivedFrames", TRUE))
            {
                *puLong= pGponGem->ReceivedFrames;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponGemEthFlowIngress_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_GEM_CTRL_T* pGponCtrl = (DML_GEM_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_GEM* pGponGem = &(pGponCtrl->dml);
            if( AnscEqualString(ParamName, "Tagged", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Ingress.Tagged;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponGemEthFlowIngressSvlan_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_GEM_CTRL_T* pGponCtrl = (DML_GEM_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_GEM* pGponGem = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Vid", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Ingress.SVLAN.Vid;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Pcp", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Ingress.SVLAN.Pcp;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Dei", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Ingress.SVLAN.Dei;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponGemEthFlowIngressCvlan_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_GEM_CTRL_T* pGponCtrl = (DML_GEM_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_GEM* pGponGem = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Vid", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Ingress.CVLAN.Vid;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Pcp", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Ingress.CVLAN.Pcp;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Dei", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Ingress.CVLAN.Dei;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponGemEthFlowEgress_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_GEM_CTRL_T* pGponCtrl = (DML_GEM_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_GEM* pGponGem = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Tagged", TRUE))
            {
                *puLong = pGponGem->EthernetFlow.Egress.Tagged;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponGemEthFlowEgressSvlan_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_GEM_CTRL_T* pGponCtrl = (DML_GEM_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_GEM* pGponGem = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Vid", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Egress.SVLAN.Vid;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Pcp", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Egress.SVLAN.Pcp;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Dei", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Egress.SVLAN.Dei;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponGemEthFlowEgressCvlan_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_GEM_CTRL_T* pGponCtrl = (DML_GEM_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_GEM* pGponGem = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Vid", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Egress.CVLAN.Vid;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Pcp", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Egress.CVLAN.Pcp;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Dei", TRUE))
            {
                *puLong= pGponGem->EthernetFlow.Egress.CVLAN.Dei;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}


BOOL GponOmci_GetParamIntValue(ANSC_HANDLE hInsContext,char* ParamName,int* pInt)
{
    BOOL ret = FALSE;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        DML_OMCI* pGponOmci = &(pGponDmlData->gpon.Omci);
        if ( GponHal_get_omci(pGponOmci) == ANSC_STATUS_SUCCESS)
        {
            if( AnscEqualString(ParamName, "RxBaseLineMessageCountValid", TRUE))
            {
                *pInt = (int) pGponOmci->RxBaseLineMessageCountValid;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "RxExtendedMessageCountValid", TRUE))
            {
                *pInt = (int) pGponOmci->RxExtendedMessageCountValid;
                ret = TRUE;
            }
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return ret;
}

BOOL GponOmci_GetParamUlongValue(ANSC_HANDLE hInsContext, char* ParamName, ULONG* puLong)
{
    BOOL ret = FALSE;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        DML_OMCI* pGponOmci = &(pGponDmlData->gpon.Omci);
        if ( GponHal_get_omci(pGponOmci) == ANSC_STATUS_SUCCESS)
        {
            if( AnscEqualString(ParamName, "MicErrors", TRUE))
            {
                *puLong = pGponOmci->MicErrors;
                ret = TRUE;
            }
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return ret;
}

BOOL GponVeip_IsUpdated(ANSC_HANDLE hInsContext)
{
    ANSC_STATUS ret = ANSC_STATUS_SUCCESS;
    BOOL        bIsUpdated = FALSE;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        ret = GponHal_get_veip(&(pGponDmlData->gpon.Veip));
        if(ret == ANSC_STATUS_SUCCESS)
        {
            bIsUpdated = TRUE;
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return bIsUpdated;
}

ULONG GponVeip_Synchronize(ANSC_HANDLE hInsContext)
{

    ANSC_STATUS            returnStatus   = ANSC_STATUS_SUCCESS;

    return returnStatus;
}

ULONG GponVeip_GetEntryCount(ANSC_HANDLE hInsContext)
{
    ULONG count = 0;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        count = pGponDmlData->gpon.Veip.ulQuantity;

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return count;
}

ANSC_HANDLE GponVeip_GetEntry(ANSC_HANDLE hInsContext, ULONG nIndex, ULONG* pInsNumber)
{
    ANSC_HANDLE pDmlEntry = NULL;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        if(nIndex < pGponDmlData->gpon.Veip.ulQuantity)
        {
            DML_VEIP_CTRL_T* pGponData = pGponDmlData->gpon.Veip.pdata[nIndex];
            if(pGponData != NULL)
            {
                *pInsNumber = nIndex + 1;
                pDmlEntry = (ANSC_HANDLE) pGponData;
            }
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return pDmlEntry;
}

BOOL GponVeip_Validate(ANSC_HANDLE hInsContext,char* pReturnParamName,ULONG* puLength)
{
    return TRUE;
}

ULONG GponVeip_Commit(ANSC_HANDLE hInsContext)
{
    return 0;
}

ULONG GponVeip_Rollback(ANSC_HANDLE hInsContext)
{
    return 0;
}

ULONG GponVeip_GetParamStringValue(ANSC_HANDLE hInsContext,char* ParamName,char* pValue,ULONG* pUlSize)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    ULONG ret = -1;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "InterDomainName", TRUE))
            {
                AnscCopyString(pValue, pGponVeip->InterDomainName);
                ret = 0;
            }
            else if( AnscEqualString(ParamName, "InterfaceName", TRUE))
            {
                AnscCopyString(pValue, pGponVeip->InterfaceName);
                ret = 0;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponVeip_GetParamUlongValue(ANSC_HANDLE hInsContext, char* ParamName, ULONG* puLong)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "MeId", TRUE))
            {
                *puLong = pGponVeip->MeId;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "AdministrativeState", TRUE))
            {
                *puLong = pGponVeip->AdministrativeState;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "OperationalState", TRUE))
            {
                *puLong = pGponVeip->OperationalState;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponVeipEthFlowIngress_GetParamUlongValue(ANSC_HANDLE hInsContext, char* ParamName, ULONG* puLong)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Tagged", TRUE))
            {
                *puLong= pGponVeip->EthernetFlow.Ingress.Tagged;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponVeipEthFlowIngress_SetParamUlongValue(ANSC_HANDLE hInsContext, char* ParamName, ULONG uValue)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    char strValue[JSON_MAX_VAL_ARR_SIZE]={0};
    char strName[JSON_MAX_STR_ARR_SIZE]={0};
    char taggedValue[3][16]={"Untagged","Single","Double"};
    BOOL ret = FALSE;


    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            snprintf(strValue,JSON_MAX_VAL_ARR_SIZE,"%ld",taggedValue[uValue]);

            /* check the parameter name and return the corresponding value */
            if( AnscEqualString(ParamName, "Tagged", TRUE))
            {
                snprintf(strName,JSON_MAX_STR_ARR_SIZE,GPON_HAL_VEIP_ETH_INGRESS_TAGGED,pGponVeip->uInstanceNumber);
                if (GponHal_setParam(strName, PARAM_STRING, strValue) == ANSC_STATUS_SUCCESS)
                {
                    pGponVeip->EthernetFlow.Ingress.Tagged = uValue;
                    ret = TRUE;
                }
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponVeipEthFlowIngressQlan_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Vid", TRUE))
            {
                *puLong= pGponVeip->EthernetFlow.Ingress.QVLAN.Vid;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Pcp", TRUE))
            {
                *puLong= pGponVeip->EthernetFlow.Ingress.QVLAN.Pcp;
                ret = TRUE;
            }
            else if( AnscEqualString(ParamName, "Dei", TRUE))
            {
                *puLong= pGponVeip->EthernetFlow.Ingress.QVLAN.Dei;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponVeipEthFlowIngressQlan_SetParamUlongValue(ANSC_HANDLE hInsContext, char* ParamName,ULONG uValue)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    char strValue[JSON_MAX_VAL_ARR_SIZE]={0};
    char strName[JSON_MAX_STR_ARR_SIZE]={0};
    BOOL ret = FALSE;


    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            snprintf(strValue,JSON_MAX_VAL_ARR_SIZE,"%ld",uValue);

            /* check the parameter name and return the corresponding value */
            if( AnscEqualString(ParamName, "Vid", TRUE))
            {
                snprintf(strName,JSON_MAX_STR_ARR_SIZE,GPON_HAL_VEIP_ETH_INGRESS_QVLAN_VID,pGponVeip->uInstanceNumber);
                if (GponHal_setParam(strName,PARAM_UNSIGNED_INTEGER,strValue) == ANSC_STATUS_SUCCESS)
                {
                    pGponVeip->EthernetFlow.Ingress.QVLAN.Vid = uValue;
                    ret = TRUE;
                }
            }
            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponVeipEthFlowEgress_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Tagged", TRUE))
            {
                *puLong= pGponVeip->EthernetFlow.Egress.Tagged;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponVeipEthFlowEgress_SetParamUlongValue(ANSC_HANDLE hInsContext, char* ParamName, ULONG uValue)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    char strValue[JSON_MAX_VAL_ARR_SIZE]={0};
    char strName[JSON_MAX_STR_ARR_SIZE]={0};
    char taggedValue[3][16]={"Untagged","Single","Double"};
    BOOL ret = FALSE;


    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            snprintf(strValue,JSON_MAX_VAL_ARR_SIZE,"%ld",taggedValue[uValue]);

            /* check the parameter name and return the corresponding value */
            if( AnscEqualString(ParamName, "Tagged", TRUE))
            {
                snprintf(strName,JSON_MAX_STR_ARR_SIZE,GPON_HAL_VEIP_ETH_EGRESS_TAGGED,pGponVeip->uInstanceNumber);
                if (GponHal_setParam(strName, PARAM_STRING, strValue) == ANSC_STATUS_SUCCESS)
                {
                    pGponVeip->EthernetFlow.Ingress.Tagged = uValue;
                    ret = TRUE;
                }
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}


BOOL GponVeipEthFlowEgressQlan_GetParamUlongValue(ANSC_HANDLE hInsContext,char* ParamName,ULONG* puLong)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    BOOL ret = FALSE;

    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            if( AnscEqualString(ParamName, "Vid", TRUE))
            {
                *puLong= pGponVeip->EthernetFlow.Egress.QVLAN.Vid;
                ret = TRUE;
            }
            if( AnscEqualString(ParamName, "Pcp", TRUE))
            {
                *puLong= pGponVeip->EthernetFlow.Egress.QVLAN.Pcp;
                ret = TRUE;
            }
            if( AnscEqualString(ParamName, "Dei", TRUE))
            {
                *puLong= pGponVeip->EthernetFlow.Egress.QVLAN.Dei;
                ret = TRUE;
            }

            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

BOOL GponVeipEthFlowEgressQlan_SetParamUlongValue(ANSC_HANDLE hInsContext, char* ParamName,ULONG uValue)
{
    DML_VEIP_CTRL_T* pGponCtrl = (DML_VEIP_CTRL_T*) hInsContext;
    char strValue[JSON_MAX_VAL_ARR_SIZE]={0};
    char strName[JSON_MAX_STR_ARR_SIZE]={0};
    BOOL ret = FALSE;


    if(pGponCtrl != NULL)
    {
        GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
        if(pGponDmlData != NULL)
        {
            DML_VEIP* pGponVeip = &(pGponCtrl->dml);

            snprintf(strValue,JSON_MAX_VAL_ARR_SIZE,"%ld",uValue);

            /* check the parameter name and return the corresponding value */
            if( AnscEqualString(ParamName, "Vid", TRUE))
            {
                snprintf(strName,JSON_MAX_STR_ARR_SIZE,GPON_HAL_VEIP_ETH_EGRESS_QVLAN_VID,pGponVeip->uInstanceNumber);
                if (GponHal_setParam(strName,PARAM_UNSIGNED_INTEGER,strValue) == ANSC_STATUS_SUCCESS)
                {
                    pGponVeip->EthernetFlow.Egress.QVLAN.Vid = uValue;
                    ret = TRUE;
                }
            }
            GponMgrDml_GetData_release(pGponDmlData);
        }
    }

    return ret;
}

ULONG Gpontr69_GetParamStringValue(ANSC_HANDLE hInsContext,char* ParamName,char* pValue,ULONG* pUlSize)
{
    ULONG ret = -1;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        DML_TR69* pGponTr69 = &(pGponDmlData->gpon.Tr69);
        if( GponHal_get_tr69(pGponTr69) == ANSC_STATUS_SUCCESS)
        {
            if( AnscEqualString(ParamName, "url", TRUE))
            {
                AnscCopyString(pValue, pGponTr69->url);

                ret = 0;
            }
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return ret;
}

BOOL Gpontr69_GetParamUlongValue(ANSC_HANDLE hInsContext, char* ParamName, ULONG* puLong)
{
    BOOL ret = FALSE;

    GPON_DML_DATA* pGponDmlData = GponMgrDml_GetData_locked();
    if(pGponDmlData != NULL)
    {
        DML_TR69* pGponTr69 = &(pGponDmlData->gpon.Tr69);

        if( AnscEqualString(ParamName, "AssociatedTag", TRUE))
        {
            *puLong= pGponTr69->AssociatedTag;

            ret = TRUE;
        }

        GponMgrDml_GetData_release(pGponDmlData);
    }

    return ret;
}

