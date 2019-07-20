#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include "rh4n.h"

#ifndef AWS
#include "rh4n_jni_environ.h"
#endif

#include "rh4n_natcall.h"
#include "rh4n_utils.h"

#ifndef AWS
int rh4nNaturalcreateProcess(RH4nProperties *props, RH4NEnvirons *environs, char *error_str) {
#else
int rh4nNaturalcreateProcess(RH4nProperties *props, char *error_str) {
#endif
    pid_t naturalpid = 0, receivedpid = 0;
    int processret = 0;

    rh4n_log_info(props->logging, "Create natural process");
    if((naturalpid = fork()) == -1) {
        rh4n_log_error(props->logging, "Creating natural process failed - %s", strerror(errno));
        sprintf(error_str, "Could not create natural process. Fork failed - %s", strerror(errno));
        return(RH4N_RET_INTERNAL_ERR);
    }

    if(naturalpid == 0) {
        int natret = 0;
#ifndef AWS
        natret = rh4nCallNatural(props, environs);
#else
        natret = rh4nCallNatural(props);
#endif
        rh4n_log_debug(props->logging, "Exiting with status: %d", natret);
        exit(natret);
    }

    rh4n_log_info(props->logging, "Waiting for Natural process. PID: [%d]", naturalpid);
    receivedpid = waitpid(naturalpid, &processret, 0x00);
    processret = WEXITSTATUS(processret);
    rh4n_log_info(props->logging, "PID [%d] exited with state [%d]", receivedpid, processret);

    return(processret);
}


#ifndef AWS
int rh4nCallNatural(RH4nProperties *props, RH4NEnvirons *environs) {
#else
int rh4nCallNatural(RH4nProperties *props) {
#endif
    struct parameter_description* naturalparms = NULL; 
    void *nnisharedlib = NULL;
    pnni_611_functions nnifuncs = NULL;
    char error_str[2048];
    int nniret = 0, natret = RH4N_RET_OK;
    struct natural_exception naturalex;

    if(rh4nUtilsloadSharedLibrary(props, RH4N_SONAME_NATURAL, &nnisharedlib, error_str) != RH4N_RET_OK) {
        rh4n_log_error(props->logging, "So loading: %s", error_str);
        rh4nlogInternalError(props, error_str);
        return(RH4N_RET_INTERNAL_ERR);
    }

    if((nnifuncs = rh4nUtilsgetNNIFunctions(props, nnisharedlib, error_str)) == NULL) {
        rh4n_log_error(props->logging, "Could not load NNI functions - %s", error_str);
        rh4nlogInternalError(props, error_str);
        return(RH4N_RET_INTERNAL_ERR);
    }

    rh4n_log_info(props->logging, "Init natural session with [%s]", props->natparms);
    if((nniret = nnifuncs->pf_nni_initialize(nnifuncs, props->natparms, NULL, NULL)) != NNI_RC_OK) {
        rh4n_log_error(props->logging, "natural init failed. NNI ret: %d", nniret);
        rh4nlogInternalError(props, "Could not init natural session. Please check natural parameters");
        return(RH4N_RET_INTERNAL_ERR);
    }
    rh4n_log_debug(props->logging, "Successfully init natural session");

    if((naturalparms = rh4nNaturalParamsGeneration(nnifuncs, props, &nniret)) == NULL) {
        rh4n_log_error(props->logging, "Could not init natural parameters. NNI err: [%d]", nniret);
        return(RH4N_RET_INTERNAL_ERR);
    }
    rh4n_log_debug(props->logging, "Successfully init natural parameters: [%p]", nnifuncs);

    rh4n_log_info(props->logging, "Logon into Library [%s]", props->natlibrary);
    if((nniret = nnifuncs->pf_nni_logon(nnifuncs, props->natlibrary, NULL, NULL)) != NNI_RC_OK) {
        rh4n_log_error(props->logging, "Could not logon to library [%s]. NNI ret: %d", nniret);
        sprintf(error_str, "Could not logon to library [%s]. Please check library name or pemissions", props->natlibrary);
        rh4nlogInternalError(props, error_str);
        return(RH4N_RET_INTERNAL_ERR);
    }
    rh4n_log_debug(props->logging, "Successfully logedon to library");

#ifndef AWS
    if(rh4nEnvironPutAll(environs) != RH4N_RET_OK) {
        rh4n_log_error(props->logging, "Could not set enviroment variables");
        return(RH4N_RET_INTERNAL_ERR);
    }
#endif

    rh4n_log_info(props->logging, "Calling natural program [%s]", props->natprogram);
    if((nniret = nnifuncs->pf_nni_callnat(nnifuncs, props->natprogram, 2, naturalparms, &naturalex)) != NNI_RC_OK) {
        rh4nHandleNaturalError(props, nniret, naturalex);
        natret = RH4N_RET_INTERNAL_ERR;
    }

    rh4n_log_info(props->logging, "Uninitialize NNI");
    if((nniret = nnifuncs->pf_nni_uninitialize(nnifuncs)) != NNI_RC_OK) {
        rh4n_log_error(props->logging, "Error while uninitialize NNI. NNI err: [%d]", nniret);
        return(RH4N_RET_INTERNAL_ERR);
    }

    rh4n_log_info(props->logging, "Freeing NNI");
    if((nniret = nnifuncs->pf_nni_free_interface(nnifuncs)) != NNI_RC_OK) {
        rh4n_log_error(props->logging, "Error while freeing NNI. NNI err: [%d]", nniret);
        return(RH4N_RET_INTERNAL_ERR);
    }

    rh4n_log_debug(props->logging, "Everyting went fine exiting natural process");

    rh4nUtilscloseSharedLibrary(nnisharedlib);

    return(natret);
}
