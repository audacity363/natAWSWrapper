#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "natuser.h"
#include "natni.h"

#include "rh4n.h"
#include "rh4n_nat_varhandling.h"

int rh4nnatputString(RH4nNatVarHandleParms *args, char *groupname, char *varname) {
    int rc = 0, strlength = 0;
    char *buff = NULL;

    if(args->desc->dimensions > 0) return(rh4nnatputStringArray(args, groupname, varname));
    
    if((rc = rh4nvarGetStringLength(args->varlist, groupname, varname, &strlength)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not get strlength for %s.%s. Varlib return: %d", groupname, varname, rc);
        return(rc);
    }

    if(!(args->desc->flags & NNI_FLG_DYNAMIC)) {
        if(args->desc->length < strlength) 
            return(RH4N_RET_BUFFER_OVERFLOW);
    }

    if((buff = malloc(sizeof(char)*(strlength+1))) == NULL) {
        return(RH4N_RET_MEMORY_ERR);
    }

    if((rc = rh4nvarGetString(args->varlist, groupname, varname, strlength+1, buff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not get string value from %s.%s. Varlib return: %d", groupname, varname, rc);
        free(buff);
        return(rc);
    }
    
    if((rc = args->nnifuncs->pf_nni_put_parm(args->nnifuncs, args->parmindex, args->parmhandle, strlength, buff)) != NNI_RC_OK && rc < 0) {
        free(buff);
        sprintf(args->errorstr, "Could not set parm %d. NNI return: %d", args->parmindex, rc);
        return(rc);
    }
    free(buff);

    return(RH4N_RET_OK);
}

int rh4nnatputStringArray(RH4nNatVarHandleParms *args, char* groupname, char* varname) {
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
            if((rc = rh4nnatputStringArrayEntry(args, groupname, varname, arrindex)) != RH4N_RET_OK) {
                return(rc);
            }
        } else {
            for(y=0; y < arrlength[1]; y++) {
                arrindex[1] = y;
                if(args->desc->dimensions == 2) {
                    if((rc = rh4nnatputStringArrayEntry(args, groupname, varname, arrindex)) != RH4N_RET_OK) {
                        return(rc);
                    }
                } else {
                    for(z=0; z < arrlength[2]; z++) {
                        arrindex[2] = z;
                        if((rc = rh4nnatputStringArrayEntry(args, groupname, varname, arrindex)) != RH4N_RET_OK) {
                            return(rc);
                        }
                    }
                }
            }
        }
    }
    return(RH4N_RET_OK);
}

int rh4nnatputStringArrayEntry(RH4nNatVarHandleParms *args, char* groupname, char* varname, int index[3]) {
    int strlength = 0, rc = 0;
    char *buff = NULL;

    if((rc = rh4nvarGetStringLengthArrayEntry(args->varlist, groupname, varname, index, &strlength)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not get string length from %s.%s x: %d y: %d z: %d. Varlib return: %d", groupname, 
            varname, index[0], index[1], index[2], rc);
        return(rc);
    }

    if(!(args->desc->flags & NNI_FLG_DYNAMIC)) {
        if(args->desc->length < strlength) 
            return(RH4N_RET_BUFFER_OVERFLOW);
    }

    if((buff = malloc((strlength+1)*sizeof(char))) == NULL) {
        sprintf(args->errorstr, "Could not allocate memory for string buff");
        return(RH4N_RET_MEMORY_ERR);
    }

    if((rc = rh4nvarGetStringArrayEntry(args->varlist, groupname, varname, index, strlength+1, buff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not get string from %s.%s x: %d y: %d z: %d. Varlib return: %d", groupname, 
            varname, index[0], index[1], index[2], rc);
        free(buff);
        return(rc);
    }

    if((rc = args->nnifuncs->pf_nni_put_parm_array(args->nnifuncs, args->parmindex, args->parmhandle, strlen(buff), 
            buff, index)) != NNI_RC_OK) {
        if(rc < 0) {
            free(buff);
            return(RH4N_RET_NNI_ERR);
        }
    }
    return(RH4N_RET_OK);
}
