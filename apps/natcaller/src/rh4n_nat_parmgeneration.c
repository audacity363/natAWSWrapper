#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_natcall.h"

struct parameter_description* rh4nNaturalParamsGeneration(pnni_611_functions nnifuncs, RH4nProperties *properties, int *pnnierr) {
    void *pvnniparms = NULL;
    int nniret = 0;
    static struct parameter_description nniparms[2];

    rh4n_log_info(properties->logging, "Start creating natural parameter set");

    if((nniret = nnifuncs->pf_nni_create_parm(nnifuncs, 2, &pvnniparms)) != NNI_RC_OK) {
        rh4n_log_error(properties->logging, "Could not init natural parms. Ret = %d", nniret);
        *pnnierr = nniret;
        return(NULL);
    }

    //Set natural parm zero
    if((nniret = nnifuncs->pf_nni_init_parm_d(nnifuncs, 0, pvnniparms, NNI_TYPE_ALPHA, NNI_FLG_PROTECTED)) != NNI_RC_OK) {
        rh4n_log_error(properties->logging, "Could not create natural parm 0. Ret = %d", nniret);
        *pnnierr = nniret;
        return(NULL);
    }

    if((nniret = nnifuncs->pf_nni_put_parm(nnifuncs, 0, pvnniparms, strlen(properties->httpreqtype), properties->httpreqtype)) != NNI_RC_OK) {
        rh4n_log_error(properties->logging, "Could not set value for natural parm 0. Ret = %d", nniret);
        *pnnierr = nniret;
        return(NULL);
    }

    //Set natural parm one
    if((nniret = nnifuncs->pf_nni_init_parm_d(nnifuncs, 1, pvnniparms, NNI_TYPE_BIN, NNI_FLG_PROTECTED)) != NNI_RC_OK) {
        rh4n_log_error(properties->logging, "Could not create natural parm 1. Ret = %d", nniret);
        *pnnierr = nniret;
        return(NULL);
    }

    if((nniret = nnifuncs->pf_nni_put_parm(nnifuncs, 1, pvnniparms, sizeof(RH4nProperties*), &properties)) != NNI_RC_OK) {
        rh4n_log_error(properties->logging, "Could not set value for natural parm 1. Ret = %d", nniret);
        *pnnierr = nniret;
        return(NULL);
    }

    if((nniret = nnifuncs->pf_nni_get_parm_info(nnifuncs, 0, pvnniparms, &nniparms[0])) != NNI_RC_OK) {
        rh4n_log_error(properties->logging, "Could not get parameter description of parameter 0. Ret = %d", nniret);
        *pnnierr = nniret;
        return(NULL);
    }

    if((nniret = nnifuncs->pf_nni_get_parm_info(nnifuncs, 1, pvnniparms, &nniparms[1])) != NNI_RC_OK) {
        rh4n_log_error(properties->logging, "Could not get parameter description of parameter 1. Ret = %d", nniret);
        *pnnierr = nniret;
        return(NULL);
    }
    return(nniparms);
}
