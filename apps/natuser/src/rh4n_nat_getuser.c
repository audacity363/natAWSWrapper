#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_nat.h"

long rh4nnatGetUser(WORD nparms, void *parmhandle, void *traditional) {
    if(traditional)  return(RH4N_RET_TARADITIONAL); 
    if(nparms <= 1)  return(RH4N_RET_PARM_MISSMATCH);

    char errorstr[2048];
    int utilsret = 0, strlength = 0;
    RH4nNatInit_t initparms = {NULL, NULL, NULL, NULL};
    struct parameter_description var_description;

    if((utilsret = rh4nnatInit(parmhandle, &initparms, errorstr, false)) != RH4N_RET_OK) {
        return(utilsret);
    }

    rh4n_log_debug(initparms.props->logging, "Sucessfully got default parms.");

    strlength = strlen(initparms.props->username);

    if(strlength == 0) {
        rh4n_log_info(initparms.props->logging, "Username is not set.");
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        return(RH4N_RET_NO_USER);
    }

    if((utilsret = initparms.nnifuncs->pf_nni_get_parm_info(initparms.nnifuncs, 1, 
                    parmhandle, &var_description)) != NNI_RC_OK) {
        rh4n_log_error(initparms.props->logging, "Could not get natural username variable. NNI return: %d", utilsret);
        return(RH4N_RET_NNI_ERR);
    }

    if(!(var_description.flags & NNI_FLG_DYNAMIC)) {
        if(var_description.length < strlength)
            rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
            return(RH4N_RET_BUFFER_OVERFLOW);
    }


    if((utilsret = initparms.nnifuncs->pf_nni_put_parm(initparms.nnifuncs, 1, parmhandle, 
            strlength, initparms.props->username)) != NNI_RC_OK) {
        rh4n_log_error(initparms.props->logging, "Could not set natural username variable. NNI return: %d", utilsret);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        return(RH4N_RET_NNI_ERR);
    }

    rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
    return(RH4N_RET_OK);
}
