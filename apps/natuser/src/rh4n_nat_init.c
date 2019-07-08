#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_nat.h"
#include "rh4n_utils.h"

int rh4nnatInit(void *parmhandle, RH4nNatInit_t *initparms, char *errorstr, bool checkformat) {
    int utilsret = 0;

    if((utilsret = rh4nUtilsloadSharedLibrary(NULL, RH4N_SONAME_NATURAL, &initparms->sharedlibrary, 
                                              errorstr)) != RH4N_RET_OK) { 
        fprintf(stderr, "Loading [%s] failed: [%s]\n", RH4N_SONAME_NATURAL, errorstr); fflush(stderr);
        return(utilsret); 
    }


    if((initparms->nnifuncs = rh4nUtilsgetNNIFunctions(NULL, initparms->sharedlibrary, errorstr)) == NULL) {
        fprintf(stderr, "Loading NNI function failed: [%s]\n", errorstr); fflush(stderr);
        rh4nUtilscloseSharedLibrary(initparms->sharedlibrary);
        return(RH4N_RET_NNI_ERR);
    }

    if((utilsret = rh4nnatGetParameter(initparms->nnifuncs, parmhandle, &initparms->props, 
                                       &initparms->formatstr, errorstr, checkformat)) != RH4N_RET_OK) {
        fprintf(stderr, "Could not get parms: %d\n", utilsret); fflush(stderr);
        return(utilsret);
    }

    return(RH4N_RET_OK);
}
