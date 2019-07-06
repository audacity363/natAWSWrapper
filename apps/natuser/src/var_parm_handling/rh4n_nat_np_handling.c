#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_nat.h"
#include "rh4n_nat_varhandling.h"

void rh4nnatStripSpaces(char *target);

int rh4nnatNumericPackedHandler(RH4nNatVarHandleParms *args) {
    int varlibret = 0, nniret = 0, bufflength = 0;
    void *tmpbuff = NULL;
    char strbuff[100];

    if(args->desc->dimensions > 0) {
        return(rh4nnatNumericPackedArrayHandler(args));
    }

    bufflength = args->desc->byte_length;
    
    if((tmpbuff = malloc(bufflength)) == NULL) return(RH4N_RET_MEMORY_ERR);
    memset(tmpbuff, 0x00, bufflength);

    if((nniret = args->nnifuncs->pf_nni_get_parm(args->nnifuncs, args->parmindex, args->parmhandle, 
        bufflength, tmpbuff)) != NNI_RC_OK) {
        free(tmpbuff);
        sprintf(args->errorstr, "Could not get parm %d. NNI ret: %d", args->parmindex, nniret);
        return(RH4N_RET_NNI_ERR);
    }
    memset(strbuff, 0x00, sizeof(strbuff));
    
    if((nniret = args->nnifuncs->pf_nni_to_string(args->nnifuncs, bufflength, tmpbuff, args->desc->format,
        args->desc->length_all, args->desc->precision, sizeof(strbuff)-1, strbuff)) != NNI_RC_OK) {
        free(tmpbuff);
        sprintf(args->errorstr, "Could not convert %c to String. NNI ret: %d", args->desc->format, nniret);
        return(RH4N_RET_NNI_ERR);
    }
    rh4nUtilsTrimSpaces(strbuff);
    rh4n_log_debug(args->props->logging, "God converted string: [%s]", strbuff);

    rh4nnatStripSpaces(strbuff);
    rh4n_log_debug(args->props->logging, "God striped string: [%s]", strbuff);

    if((varlibret = rh4nvarCreateNewString(args->varlist, NULL, args->tmpname, strbuff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not create new string: %d", varlibret);
        free(tmpbuff);
        return(varlibret);
    }

    free(tmpbuff);
    return(RH4N_RET_OK);
}

int rh4nnatNumericPackedArrayHandler(RH4nNatVarHandleParms *args) {
    int x = 0, y = 0, z = 0, varlibret = 0, arrindex[3] = {0, 0, 0};

    if((varlibret = rh4nvarCreateNewStringArray(args->varlist, NULL, args->tmpname, args->desc->dimensions, 
                args->desc->occurrences)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Error while creating String Array: %d", varlibret);
        return(varlibret);
    }

    for(; x < args->desc->occurrences[0]; x++) {
        arrindex[0] = x; arrindex[1] = arrindex[2] = 0;
        if(args->desc->dimensions == 1) {
            if((varlibret = rh4nnatSaveNumericPackedArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
        } else {
            for(y = 0; y < args->desc->occurrences[1]; y++) {
                arrindex[1] = y; arrindex[2] = 0;
                if(args->desc->dimensions == 2) {
                    if((varlibret = rh4nnatSaveNumericPackedArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
                } else {
                    for(z = 0; z < args->desc->occurrences[2]; z++) {
                        arrindex[2] = z;
                        if((varlibret = rh4nnatSaveNumericPackedArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
                    }
                }
            }
        }
    }

    return(RH4N_RET_OK);
}

int rh4nnatSaveNumericPackedArrayEntry(RH4nNatVarHandleParms *args, int index[3]) {
    int nniret = 0, varlibret = 0, varlibindex[3] = { -1, -1, -1};
    char strbuff[100], tmpbuff[MAX_NAT_N_SIZE];

    memset(tmpbuff, 0x00, sizeof(tmpbuff));
    memset(strbuff, 0x00, sizeof(strbuff));

    if((nniret = args->nnifuncs->pf_nni_get_parm_array(args->nnifuncs, args->parmindex, args->parmhandle, sizeof(tmpbuff), 
            tmpbuff, index)) != NNI_RC_OK && nniret < 0) {
        sprintf(args->errorstr, "Could not get array entry from parm %d (x: %d, y:%d, z: %d) NNI err: %d", 
            args->parmindex, index[0], index[1], index[2], nniret);
        rh4n_log_error(args->props->logging, "%s", args->errorstr);
        return(RH4N_RET_NNI_ERR);

    }

    rh4n_log_debug(args->props->logging, "God nummeric: [%s]", tmpbuff);

    if((nniret = args->nnifuncs->pf_nni_to_string(args->nnifuncs, sizeof(tmpbuff), tmpbuff, args->desc->format,
        args->desc->length+args->desc->precision, args->desc->precision, sizeof(strbuff), strbuff)) != NNI_RC_OK) {
        sprintf(args->errorstr, "Could not convert %c to String. NNI ret: %d", args->desc->format, nniret);
        return(RH4N_RET_NNI_ERR);
    }

    rh4nnatStripSpaces(strbuff);

    if(args->desc->dimensions == 1) varlibindex[0] = index[0];
    else if(args->desc->dimensions == 2) { varlibindex[0] = index[0]; varlibindex[1] = index[1]; }
    else if(args->desc->dimensions == 3) { varlibindex[0] = index[0]; varlibindex[1] = index[1]; varlibindex[2] = index[2]; }

    rh4nUtilsTrimSpaces(strbuff);
    if((varlibret = rh4nvarSetStringArrayEntry(args->varlist, NULL, args->tmpname, varlibindex, strbuff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not set Array Entry x: %d y: %d z: %d", varlibindex[0], varlibindex[1], varlibindex[2]);
        return(varlibret);
    }

    return(RH4N_RET_OK);
}


void rh4nnatStripSpaces(char *target) {
    int i = 0;

    for(; i < strlen(target); i++) {
        if(target[i] != ' ') break;
    }

    memmove(target, target+i, strlen(target)-(i-1));
}
