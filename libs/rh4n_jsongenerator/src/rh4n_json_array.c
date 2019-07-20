#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>

#include "rh4n.h"
#include "rh4n_json.h"

#define RH4N_JSON_DIM_NOT_INIT -1
#define RH4N_JSON_DIM_NOT_SET -2


bool rh4njsonCheckIfJSONArray(RH4nVarEntry_t *target, RH4nProperties *props, short ignoreGRP) {
    RH4nVarEntry_t *hptr = NULL;

    for(hptr = target; hptr != NULL; hptr = hptr->next) {
        rh4n_log_debug(props->logging, "Checking [%s]", hptr->name);
        if(hptr->var.type == RH4NVARTYPEGROUP && ignoreGRP == 1) {
            if(rh4njsonCheckIfJSONArray(hptr->nextlvl, props, ignoreGRP) == false) {
                return(false);
            }
        } else if(hptr->var.type != RH4NVARTYPEARRAY) {
            rh4n_log_debug(props->logging, "JSON Array Check = false. [%s] is not an array", hptr->name);
            return(false);
        }
    }
    return(true);
}

void rh4njsonPrintJSONArray(RH4nVarObj *target, FILE *outputfile, RH4nProperties *props) {
    RH4nVarObj *hptr = NULL;
    int i = 0;
   
    fwprintf(outputfile, L"[") ;
    for(; i < target->length; i++) {
        hptr = &((RH4nVarObj*)target->value)[i];
        if(hptr->type == RH4NVARTYPEARRAY) {
            rh4njsonPrintJSONArray(hptr, outputfile, props);
        } else {
            rh4nvarPrintVar(hptr, props, outputfile); 
        }
        if(i+1 < target->length) { fwprintf(outputfile, L","); }
    }
    fwprintf(outputfile, L"]") ;
}

void rh4njsonPrintJSONObjectArray(RH4nVarEntry_t *target, FILE *outputfile, RH4nProperties *props) {
    int commonDim = 0, dimOK[3] = {-1, -1, -1};
    RH4nJSONObjectArrayParms_t args = {
            -1, 
            {-1, -1, -1},
            {-1, -1, -1}
        };

    rh4njsonpregetCommonDimension(target, args.length, dimOK, props);
    args.aimdim = rh4njsongetCommonDimension(dimOK);

    rh4n_log_develop(props->logging, "Found the common dimension %d", args.aimdim);
    rh4n_log_develop(props->logging, "Length of all arrays: %d, %d, %d", args.length[0], args.length[1], args.length[2]);
    rh4n_log_develop(props->logging, "Start generating array with objects");
    fwprintf(outputfile, L"[");
    rh4njsonPrintObjectArrayEntry(target, outputfile, 1, args, props);
    fwprintf(outputfile, L"]");
}


int rh4njsonPrintObjectArrayEntry(RH4nVarEntry_t *target, FILE *outputfile, int curdim, RH4nJSONObjectArrayParms_t args, RH4nProperties *props) {
    RH4nVarEntry_t *hptr = target;
    RH4nVarObj *arrentry = NULL;
    int i = 0, varlibret = 0, arraydims = 0, loopgoal = 0;

    if(args.length[curdim-1] == -2) {
        loopgoal = 1;
    } else {
        loopgoal = args.length[curdim-1];
    }

    for(; i < loopgoal; i++) {
        if(args.length[curdim-1] != -2) {
            args.index[curdim-1] = i;
        }

        if(curdim < args.aimdim) {
            fwprintf(outputfile, L"[");
            rh4njsonPrintObjectArrayEntry(hptr, outputfile, curdim+1, args, props);
            fwprintf(outputfile, L"]");
            if(i+1 < args.length[curdim-1]) { fwprintf(outputfile, L","); }
            continue;
        }
        fwprintf(outputfile, L"{");
        for(hptr = target; hptr != NULL; hptr = hptr->next) {
            fwprintf(outputfile, L"\"%s\":", hptr->name);
            if(hptr->var.type == RH4NVARTYPEGROUP) {
                args.length[curdim] = -2;
                rh4njsonPrintObjectArrayEntry(hptr->nextlvl, outputfile, curdim+1, args, props);
                if(hptr->next != NULL) { fwprintf(outputfile, L","); }
                continue;
            } 
            
            rh4nvarGetArrayDimension(&hptr->var, &arraydims);

            rh4n_log_develop(props->logging, "Printing %s with index X: %d Y: %d Z: %d", hptr->name, args.index[0], args.index[1], args.index[2]);
            if((varlibret = rh4nvarGetArrayEntry(&hptr->var, args.index, &arrentry)) != RH4N_RET_OK) {
                rh4n_log_error(props->logging, "Could not get entry (%d, %d, %d) from %s. Varlib return: %d", 
                    args.index[0], args.index[1], args.index[2], hptr->name, varlibret);
                return(varlibret);
            }
            switch(arrentry->type) {
                case RH4NVARTYPEARRAY:
                    rh4njsonPrintJSONArray(arrentry, outputfile, props);
                    break;
                default:
                    rh4nvarPrintVar(arrentry, props, outputfile);
            }
            if(hptr->next != NULL) { fwprintf(outputfile, L","); }

        }
        fwprintf(outputfile, L"}");
        if(i+1 < args.length[curdim-1]) { fwprintf(outputfile, L","); }
    }

}

int rh4njsongetCommonDimension(int dimOK[3]) {
    int i = 0;
    for(; i < 3; i++) {
        if(dimOK[i] != 1) { break; }
    }

    return(i);
}

void rh4njsonpregetCommonDimension(RH4nVarEntry_t *target, int plength[3], int pdimOK[3], RH4nProperties *props) {
    int curlength[3] = { -1, -1, -1},
        mylength[3] = {-1, -1, -1 },
        childlength[3] = {-1, -1, -1 };

    int dimOK[3] = { -1, -1, -1 },
        childdimOK[3] = {-1, -1, -1 };
    RH4nVarEntry_t *hptr = NULL;

    for(hptr = target; hptr != NULL; hptr = hptr->next) {
        if(hptr->var.type == RH4NVARTYPEGROUP) { 
            rh4n_log_develop(props->logging, "Check dims for [%s] type [%d]", hptr->name, hptr->var.type);
            rh4njsonpregetCommonDimension(hptr->nextlvl, childlength, childdimOK, props);
            rh4njsonMergeCommonDimResults(mylength, dimOK, childlength, childdimOK, props);
            memset(childlength, -1, sizeof(childlength));
            memset(childdimOK, -1, sizeof(childdimOK));
            continue; 
        }

        rh4nvarGetArrayLength(&hptr->var, curlength);
        rh4njsoncheckCommonDimension(curlength, mylength, dimOK);
    }
    
    rh4n_log_develop(props->logging, "Dims check: [%d] [%d] [%d]", dimOK[0], dimOK[1], dimOK[2]);
    //rh4n_log_develop(props->logging, "sizeof plength: [%d] sizeof mylength: [%d]", sizeof(plength), sizeof(mylength));

    if(plength != NULL) { memcpy(plength, mylength, sizeof(mylength)); }
    if(pdimOK != NULL) { memcpy(pdimOK, dimOK, sizeof(dimOK)); }
}

void rh4njsonMergeCommonDimResults(int length1[3], int dimOK1[3], int length2[3], int dimOK2[3], RH4nProperties *props) {
    int i = 0;

    rh4n_log_develop(props->logging, "Merging length [%d] [%d] [%d] into [%d] [%d] [%d]", length2[0], length2[1], length2[2],
        length1[0], length1[1],length1[2]);

    rh4n_log_develop(props->logging, "Merging dimOK [%d] [%d] [%d] into [%d] [%d] [%d]", dimOK2[0], dimOK2[1], dimOK2[2],
        dimOK1[0], dimOK1[1], dimOK1[2]);

    if(memcmp(length1, length2, sizeof(int)*3) == 0 && memcmp(dimOK1, dimOK2, sizeof(int)*3) == 0)
        return;

    for(; i < 3; i++) {
        if((dimOK1[i] == RH4N_JSON_DIM_NOT_INIT && dimOK2[i] != RH4N_JSON_DIM_NOT_INIT) && dimOK1[i] != dimOK2[i]) {
            dimOK1[i] = dimOK2[i];
            length1[i] = length2[i];
        } else if(dimOK1[i] != dimOK2[i]) {
            dimOK1[i] = 0;
        } else if((dimOK1[i] != RH4N_JSON_DIM_NOT_INIT && dimOK2[i] != RH4N_JSON_DIM_NOT_INIT) && dimOK1[i] == dimOK2[i]) {
            dimOK1[i] = length1[i] == length2[i] ? 1 : 0;
        } 
    }
}

void rh4njsoncheckCommonDimension(int curlength[3], int length[3], int dimOK[3]) {
    int i = 0;

    for(; i < 3; i++) {
        //When length is set to DIM_NOT_SET whe are in the first interation
        //Set it to the current length
        //The dim is ok when there is a length for it set
        if(length[i] == RH4N_JSON_DIM_NOT_INIT) {
            length[i] = curlength[i] == RH4N_JSON_DIM_NOT_INIT ? RH4N_JSON_DIM_NOT_SET : curlength[i];
            dimOK[i] = curlength[i] == RH4N_JSON_DIM_NOT_INIT ? 0 : 1;
            continue;
        }

        //Compare the current length with previous length
        //When they ar equals the dim is ok otherwise not
        if(curlength[i] != length[i]) {
            dimOK[i] = 0;
        } else {
            if(dimOK[i] == RH4N_JSON_DIM_NOT_INIT) {
                dimOK[i] = 1;
            }
        }
    }
}
