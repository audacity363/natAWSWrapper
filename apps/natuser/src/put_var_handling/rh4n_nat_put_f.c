#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>

#include "natuser.h"
#include "natni.h"

#include "rh4n.h"
#include "rh4n_nat_varhandling.h"

//Not sure if this works. Can't test the return value in natural beacuse, for some reason, 
//a write work file does not support floats...
int rh4nnatputFloat(RH4nNatVarHandleParms *args, char *groupname, char *varname) {
    double val = 0;
    int rc = 0;

    if(args->desc->length == 4) {
        sprintf(args->errorstr, "F4 is too small. Use F8. Ignoring variable");
        return(RH4N_RET_USE_F8);
    }

    if(args->desc->dimensions > 0) return(rh4nnatputFloatArray(args, groupname, varname));

    if((rc = rh4nvarGetFloat(args->varlist, groupname, varname, &val)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not get float %s.%s. Varlib return: %d", groupname, varname, rc);
        return(rc);
    }

    rh4n_log_debug(args->props->logging, "Got float %f", val);

    if((rc = args->nnifuncs->pf_nni_put_parm(args->nnifuncs, args->parmindex, args->parmhandle, sizeof(double), &val)) != NNI_RC_OK) {
        sprintf(args->errorstr, "Could not set float NNI ret: %d", rc);
        return(RH4N_RET_NNI_ERR);
    }

    return(RH4N_RET_OK);
}

int rh4nnatputFloatArray(RH4nNatVarHandleParms *args, char* groupname, char* varname) {
    int rc = 0, x = 0, y = 0, z = 0, arrlength[3] = { 0, 0, 0}, arrindex[3] = { -1, -1, -1 };
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    if((rc = rh4nvarGetRef(args->varlist, groupname, varname, &_refvar)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not get variable %s.%s. Varlib return: %d", groupname, varname, rc);
        return(rc);
    }

    if((rc = rh4nvarGetArrayLength(&_refvar.var->var, arrlength)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not get array length for %s.%s. Varlib return: %d", groupname, varname, rc);
        return(rc);
    }

    if(!(args->desc->flags & NNI_FLG_UBVAR_0) && args->desc->occurrences[0] < arrlength[0]) {
        return(RH4N_RET_DIM1_TOO_SMALL);
    } else if(args->desc->dimensions > 1 && !(args->desc->flags & NNI_FLG_UBVAR_1) && args->desc->occurrences[1] < arrlength[1]) {
        return(RH4N_RET_DIM2_TOO_SMALL);
    } else if(args->desc->dimensions > 2 && !(args->desc->flags & NNI_FLG_UBVAR_2) && args->desc->occurrences[2] < arrlength[2]) {
        return(RH4N_RET_DIM3_TOO_SMALL);
    }
    
    if(args->desc->flags & NNI_FLG_XARRAY) {
        rh4n_log_debug(args->props->logging, "Resize array to [%d,%d,%d]", arrlength[0], arrlength[1], arrlength[2]);
        if((rc = args->nnifuncs->pf_nni_resize_parm_array(args->nnifuncs, args->parmindex, args->parmhandle, arrlength)) != NNI_RC_OK) {
            return(RH4N_RET_NNI_ERR);
        }
    }

    for(; x < arrlength[0]; x++) {
        arrindex[0] = x;

        if(args->desc->dimensions == 1) {
            if((rc = rh4nnatputFloatArrayEntry(args, groupname, varname, arrindex)) != RH4N_RET_OK) {
                return(rc);
            }
        } else {
            for(y=0; y < arrlength[1]; y++) {
                arrindex[1] = y;
                if(args->desc->dimensions == 2) {
                    if((rc = rh4nnatputFloatArrayEntry(args, groupname, varname, arrindex)) != RH4N_RET_OK) {
                        return(rc);
                    }
                } else {
                    for(z=0; z < arrlength[2]; z++) {
                        arrindex[2] = z;
                        if((rc = rh4nnatputFloatArrayEntry(args, groupname, varname, arrindex)) != RH4N_RET_OK) {
                            return(rc);
                        }
                    }
                }
            }
        }
    }
    return(RH4N_RET_OK);
}

int rh4nnatputFloatArrayEntry(RH4nNatVarHandleParms *args, char* groupname, char* varname, int index[3]) {
    int rc = 0;
    double val = 0;

    if((rc = rh4nvarGetFloatArrayEntry(args->varlist, groupname, varname, index, &val)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not get float from %s.%s x: %d y: %d z: %d. Varlib return: %d", groupname, 
            varname, index[0], index[1], index[2], rc);
        return(rc);
    }

    if((rc = args->nnifuncs->pf_nni_put_parm_array(args->nnifuncs, args->parmindex, args->parmhandle, sizeof(double), 
            &val, index)) != NNI_RC_OK) {
        if(rc < 0) {
            return(RH4N_RET_NNI_ERR);
        }
    }
    return(RH4N_RET_OK);
}
