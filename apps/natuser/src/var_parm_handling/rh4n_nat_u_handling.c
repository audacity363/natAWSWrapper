#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_nat.h"
#include "rh4n_nat_varhandling.h"

int rh4nnatUnicodeHandler(RH4nNatVarHandleParms *args) {
    int strlength = 0, varlibret = 0, nniret = 0, buffsize = 0;
    char *tmpbuff = NULL;
    wchar_t *wctmpbuff = NULL;

    if(args->desc->dimensions > 0) {
        return(rh4nnatUnicodeArrayHandler(args));
    }
    
    strlength = args->desc->length+1;
    buffsize = args->desc->byte_length;
    
    if((tmpbuff = malloc(buffsize)) == NULL) return(RH4N_RET_MEMORY_ERR);
    memset(tmpbuff, 0x00, buffsize);

    if((wctmpbuff = malloc(sizeof(wchar_t)*strlength)) == NULL) {
        sprintf(args->errorstr, "Could not allocate memory for wchar buffer");
        free(tmpbuff);
        return(RH4N_RET_MEMORY_ERR);
    }
    memset(wctmpbuff, 0x00, sizeof(wchar_t)*strlength);

    if(strlength > 1) {
        if((nniret = args->nnifuncs->pf_nni_get_parm(args->nnifuncs, args->parmindex, args->parmhandle, 
            buffsize, tmpbuff)) != NNI_RC_OK) {
            free(tmpbuff);
            free(wctmpbuff);
            sprintf(args->errorstr, "Could not get parm %d. NNI ret: %d", args->parmindex, nniret);
            return(RH4N_RET_NNI_ERR);
        }
        rh4nnatConvert2BTo4BString(tmpbuff, wctmpbuff, args->desc->length);
        rh4nUtilsTrimSpaces4B(wctmpbuff);
    } 

    if((varlibret = rh4nvarCreateNewUString(args->varlist, NULL, args->tmpname, wctmpbuff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not create new string: %d", varlibret);
        free(tmpbuff);
        return(varlibret);
    }

    free(tmpbuff);
    return(RH4N_RET_OK);
}

int rh4nnatUnicodeArrayHandler(RH4nNatVarHandleParms *args) {
    int x = 0, y = 0, z = 0, varlibret = 0, arrindex[3] = {0, 0, 0};

    if((varlibret = rh4nvarCreateNewUStringArray(args->varlist, NULL, args->tmpname, args->desc->dimensions, 
                args->desc->occurrences)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Error while creating String Array: %d", varlibret);
        return(varlibret);
    }

    for(; x < args->desc->occurrences[0]; x++) {
        arrindex[0] = x; arrindex[1] = arrindex[2] = 0;
        if(args->desc->dimensions == 1) {
            if((varlibret = rh4nnatSaveUnicodeArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
        } else {
            for(y = 0; y < args->desc->occurrences[1]; y++) {
                arrindex[1] = y; arrindex[2] = 0;
                if(args->desc->dimensions == 2) {
                    if((varlibret = rh4nnatSaveUnicodeArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
                } else {
                    for(z = 0; z < args->desc->occurrences[2]; z++) {
                        arrindex[2] = z;
                        if((varlibret = rh4nnatSaveUnicodeArrayEntry(args, arrindex)) != RH4N_RET_OK) return(varlibret);
                    }
                }
            }
        }
    }

    return(RH4N_RET_OK);
}

int rh4nnatSaveUnicodeArrayEntry(RH4nNatVarHandleParms *args, int index[3]) {
    int strlength = 0, nniret = 0, varlibret = 0, varlibindex[3] = { -1, -1, -1}, buffsize = 0;
    char *tmpbuff = NULL;
    wchar_t *wctmpbuff = NULL;


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
    buffsize = strlength*2;

    if((tmpbuff = malloc(buffsize)) == NULL) {
        sprintf(args->errorstr, "Could not allocate memory for tmp buffer.");
        return(RH4N_RET_MEMORY_ERR);
    }
    memset(tmpbuff, 0x00, buffsize);

    if((wctmpbuff = malloc(sizeof(wchar_t)*(strlength+1))) == NULL) {
        free(tmpbuff);
        sprintf(args->errorstr, "Could not allocate memory for wc tmp buffer.");
        return(RH4N_RET_MEMORY_ERR);
    }
    memset(wctmpbuff, 0x00, sizeof(wchar_t)*(strlength+1));

    if((nniret = args->nnifuncs->pf_nni_get_parm_array(args->nnifuncs, args->parmindex, args->parmhandle, buffsize, 
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

    rh4nnatConvert2BTo4BString(tmpbuff, wctmpbuff, strlength);
    rh4nUtilsTrimSpaces4B(wctmpbuff);

    if((varlibret = rh4nvarSetUStringArrayEntry(args->varlist, NULL, args->tmpname, varlibindex, wctmpbuff)) != RH4N_RET_OK) {
        sprintf(args->errorstr, "Could not set Array Entry x: %d y: %d z: %d", varlibindex[0], varlibindex[1], varlibindex[2]);
        free(tmpbuff);
        return(varlibret);
    }

    free(tmpbuff);
    free(wctmpbuff);

    return(RH4N_RET_OK);
}

void rh4nnatConvert2BTo4BString(char *inbuff, wchar_t *outbuff, int length) {
    int i = 0, offset = 0;
    void *v_in = NULL, *v_out = NULL, *v_tmp = NULL;

    v_in = (void*)inbuff;
    v_out = (void*)outbuff;

    for(; i < length; i++) {
        offset = (sizeof(wchar_t)*i)+2;
        v_tmp = v_out+offset;
        memcpy(v_tmp, v_in+(i*2), 2);
    }
    return;
}
