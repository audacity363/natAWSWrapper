#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_nat.h"
#include "rh4n_nat_varhandling.h"

int rh4nnatStringHandler(RH4nNatVarHandleParms *args) {
    int strlength = 0, varlibret = 0, nniret = 0;
    char *tmpbuff = NULL;

    if(args->desc->dimensions > 0) {
        return(rh4nnatStringArrayHandler(args));
    }
    
    strlength = args->desc->length + 1;
    
    if((tmpbuff = malloc(sizeof(char)*strlength)) == NULL) return(RH4N_RET_MEMORY_ERR);
    memset(tmpbuff, 0x00, sizeof(char)*strlength);

    if(strlength > 1) {
        if((nniret = args->nnifuncs->pf_nni_get_parm(args->nnifuncs, args->parmindex, args->parmhandle, 
            strlength-1, tmpbuff)) != NNI_RC_OK) {
            free(tmpbuff);
            sprintf(args->errorstr, "Could not get parm %d. NNI ret: %d", args->parmindex, nniret);
            return(RH4N_RET_NNI_ERR);
        }
    }

    rh4nUtilsTrimSpaces(tmpbuff);

    if((varlibret = rh4nvarCreateNewString(args->varlist, NULL, args->tmpname, tmpbuff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not create new string: %d", varlibret);
        free(tmpbuff);
        return(varlibret);
    }

    free(tmpbuff);
    return(RH4N_RET_OK);
}

int rh4nnatStringArrayHandler(RH4nNatVarHandleParms *args) {
    int x = 0, y = 0, z = 0, varlibret = 0, arrindex[3] = {0, 0, 0};

    if((varlibret = rh4nvarCreateNewStringArray(args->varlist, NULL, args->tmpname, args->desc->dimensions, 
                args->desc->occurrences)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Error while creating String Array: %d", varlibret);
        return(varlibret);
    }

    for(; x < args->desc->occurrences[0]; x++) {
        arrindex[0] = x; arrindex[1] = arrindex[2] = -1;
        if(args->desc->dimensions == 1) {
            if((varlibret = rh4nnatSaveStringArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
        } else {
            for(y = 0; y < args->desc->occurrences[1]; y++) {
                arrindex[1] = y; arrindex[2] = -1;
                if(args->desc->dimensions == 2) {
                    if((varlibret = rh4nnatSaveStringArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
                } else {
                    for(z = 0; z < args->desc->occurrences[2]; z++) {
                        arrindex[2] = z;
                        if((varlibret = rh4nnatSaveStringArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
                    }
                }
            }
        }
    }

    return(RH4N_RET_OK);
}

int rh4nnatSaveStringArrayEntry(RH4nNatVarHandleParms *args, int index[3]) {
    int strlength = 0, nniret = 0, varlibret = 0, varlibindex[3] = { -1, -1, -1};
    char *tmpbuff = NULL;


    if(args->desc->flags & NNI_FLG_DYNVAR) {
        if((nniret = args->nnifuncs->pf_nni_get_parm_array_length(args->nnifuncs, args->parmindex, args->parmhandle, 
                &strlength, index)) != NNI_RC_OK) {
            sprintf(args->errorstr, "Could not get array length from parm %d (x: %d, y:%d, z: %d) NNI err: %d", 
                args->parmindex, index[0], index[1], index[2], nniret);
            return(RH4N_RET_NNI_ERR);
        }
    } else {
        strlength = args->desc->length;
    }

    rh4n_log_debug(args->props->logging, "Got strlen [%d] for index x: %d y: %d z: %d", strlength, index[0], 
        index[1], index[3]);

    if(strlength == 0) {
        return(RH4N_RET_OK);
    }

    if((tmpbuff = malloc(sizeof(char)*(strlength+1))) == NULL) {
        sprintf(args->errorstr, "Could not allocate memory for tmp buffer.");
        return(RH4N_RET_MEMORY_ERR);
    }
    memset(tmpbuff, 0x00, sizeof(char)*(strlength+1));

    if((nniret = args->nnifuncs->pf_nni_get_parm_array(args->nnifuncs, args->parmindex, args->parmhandle, strlength, 
            tmpbuff, index)) != NNI_RC_OK) {
        sprintf(args->errorstr, "Could not get array entry from parm %d (x: %d, y:%d, z: %d) NNI err: %d", 
            args->parmindex, index[0], index[1], index[2], nniret);
        rh4n_log_error(args->props->logging, "%s", args->errorstr);
        free(tmpbuff);
        return(RH4N_RET_NNI_ERR);

    }

    if(args->desc->dimensions == 1) varlibindex[0] = index[0];
    else if(args->desc->dimensions == 2) { varlibindex[0] = index[0]; varlibindex[1] = index[1]; }
    else if(args->desc->dimensions == 3) { varlibindex[0] = index[0]; varlibindex[1] = index[1]; varlibindex[2] = index[2]; }

    rh4nUtilsTrimSpaces(tmpbuff);
    if((varlibret = rh4nvarSetStringArrayEntry(args->varlist, NULL, args->tmpname, varlibindex, tmpbuff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not set Array Entry x: %d y: %d z: %d", varlibindex[0], varlibindex[1], varlibindex[2]);
        free(tmpbuff);
        return(varlibret);
    }

    free(tmpbuff);

    return(RH4N_RET_OK);
}
