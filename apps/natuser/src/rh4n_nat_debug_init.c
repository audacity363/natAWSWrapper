#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_nat.h"
#include "rh4n_utils.h"

#define RH4NINITPARMCOUNT 3
//"RHDBGINI" <rh4ninternals> <loglevel & logpath ("DEBUG@/tmp/")> <program, library & natsrc ("PROG.TGAP@/natsrc/fuser")> <outputfile> 


int rh4nnatReadDebugParms(void *parmhandle, pnni_611_functions nnifuncs, RH4nProperties *props);
int rh4nnatDBLogHandler(char *logstr, RH4nProperties *props);
int rh4nnatDBLibSrcHandler(char *formatstr, RH4nProperties *props);
int rh4nnatDBOutputfileHandler(char *path, RH4nProperties *props);

long rh4nnatDebugInit(WORD nparms, void *parmhandle, void *traditional) {
    if(traditional)  return(RH4N_RET_TARADITIONAL); 
    if(nparms != RH4NINITPARMCOUNT+1)  return(RH4N_RET_PARM_MISSMATCH);

    int utilsret = 0, nniret = 0;
    char errorstr[1024];
    void *sharedlib = NULL;
    pnni_611_functions nnifuncs = NULL;
    RH4nProperties *props;



    if((props = malloc(sizeof(RH4nProperties))) == NULL) return(RH4N_RET_MEMORY_ERR);
    
    if((utilsret = rh4nUtilsloadSharedLibrary(NULL, RH4N_SONAME_NATURAL, &sharedlib,
                                              errorstr)) != RH4N_RET_OK) { 
        fprintf(stderr, "Loading [%s] failed: [%s]\n", RH4N_SONAME_NATURAL, errorstr); fflush(stderr);
        return(utilsret); 
    }

    if((nnifuncs = rh4nUtilsgetNNIFunctions(NULL, sharedlib, errorstr)) == NULL) {
        fprintf(stderr, "Loading NNI function failed: [%s]\n", errorstr); fflush(stderr);
        rh4nUtilscloseSharedLibrary(sharedlib);
        return(RH4N_RET_NNI_ERR);
    }

    memset(props, 0x00, sizeof(RH4nProperties));

    if((utilsret = rh4nnatReadDebugParms(parmhandle, nnifuncs, props)) != RH4N_RET_OK) {
        free(props);
        rh4nUtilscloseSharedLibrary(sharedlib);
        return(utilsret);
    }

    props->logging = rh4nLoggingCreateRule(props->natlibrary, props->natprogram, props->i_loglevel, props->logpath);

    if((nniret = nnifuncs->pf_nni_put_parm(nnifuncs, 0, parmhandle, sizeof(props), &props)) != NNI_RC_OK) {
        fprintf(stderr, "Could not set env pointer. NNI Err: [%d]\n", nniret);
        free(props);
        rh4nUtilscloseSharedLibrary(sharedlib);
        return(RH4N_RET_NNI_ERR);
    }

    rh4nUtilscloseSharedLibrary(sharedlib);
    return(RH4N_RET_OK);
}

int rh4nnatReadDebugParms(void *parmhandle, pnni_611_functions nnifuncs, RH4nProperties *props) {
    int nniret = 0, i = 0, x = 0;
    char *tmpbuff = NULL;
    struct parameter_description var_description;

    int (*handlerfuncs[])(char*, RH4nProperties*) = 
        {
            rh4nnatDBLogHandler,
            rh4nnatDBLibSrcHandler,
            rh4nnatDBOutputfileHandler
        };

    for(i = 1; i < RH4NINITPARMCOUNT+1; i++) {
        if((nniret = nnifuncs->pf_nni_get_parm_info(nnifuncs, i, parmhandle, &var_description)) != NNI_RC_OK) {
            fprintf(stderr, "Could not get parm info %d. NNI ret: [%d]\n", i, nniret); fflush(stderr);
            return(RH4N_RET_NNI_ERR);
        }

        if(var_description.format != NNI_TYPE_ALPHA) {
            fprintf(stderr, "Parm %d has type [%c] expected [A]\n", i, var_description.format); fflush(stderr);
            return(RH4N_RET_PARM_TYPE_MISSMATCH);
        }

        if((tmpbuff = malloc(var_description.length_all+1)) == NULL) {
            fprintf(stderr, "Could not get alloc memory.\n"); fflush(stderr);
            return(RH4N_RET_MEMORY_ERR);
        }
        
        memset(tmpbuff, 0x00, var_description.length_all+1);

        if((nniret = nnifuncs->pf_nni_get_parm(nnifuncs, i, parmhandle, 
                var_description.length_all, tmpbuff)) != NNI_RC_OK) {
            fprintf(stderr, "Could not get parm %d. NNI ret: [%d]\n", i, nniret); fflush(stderr);
            free(tmpbuff);
            return(RH4N_RET_NNI_ERR);
        }

        //When the natural string is not dynamic we have to strip the spaces on the end
        if(!(var_description.flags & IF4_FLG_DYNVAR)) {
            for(x = var_description.length_all; x > 0; x--) {
                if(tmpbuff[x] != ' ') break;
            }
            memset(tmpbuff+(x+1), 0x00, var_description.length_all-x);
        }

        handlerfuncs[i-1](tmpbuff, props);

        free(tmpbuff);
    }

    return(RH4N_RET_OK);
}

int rh4nnatDBLogHandler(char *logstr, RH4nProperties *props) {
    char *atpos = NULL;

    if((atpos = strchr(logstr, '@')) == NULL) {
        return(RH4N_RET_MALFORMED_FORMAT_STR);
    }

    *atpos = '\0';
    atpos++;
    props->i_loglevel = rh4nLoggingConvertStrtoInt(logstr);
    if((props->logpath = malloc((strlen(atpos)+1)*sizeof(char))) == NULL) {
        fprintf(stderr, "Could not allocate memory for logpath");
        return(RH4N_RET_MEMORY_ERR);

    }

    memset(props->logpath, 0x00, strlen(atpos)*sizeof(char));
    strcpy(props->logpath, atpos);

    return(RH4N_RET_OK);
}


int rh4nnatDBLibSrcHandler(char *formatstr, RH4nProperties *props) {
    char *atpos = NULL, *pointpos = NULL;

    if((atpos = strchr(formatstr, '@')) == NULL) {
        return(RH4N_RET_MALFORMED_FORMAT_STR);
    }

    *atpos = '\0';
    atpos++;

    if((props->natsrcpath = malloc((strlen(atpos)+1)*sizeof(char))) == NULL) {
        fprintf(stderr, "Could not allocate memory for logpath");
        return(RH4N_RET_MEMORY_ERR);
    }
  
    memset(props->natsrcpath, 0x00, (strlen(atpos)+1)*sizeof(char));
    strcpy(props->natsrcpath, atpos);

    if((pointpos = strchr(formatstr, '.')) == NULL) {
        free(props->natsrcpath);
        return(RH4N_RET_MALFORMED_FORMAT_STR);
    }

    *pointpos = '\0';
    pointpos++;

    if((strlen(formatstr) > NNI_LEN_MEMBER  || strlen(formatstr) == 0) ||
       (strlen(pointpos) > NNI_LEN_LIBRARY  || strlen(pointpos) == 0)) {
        free(props->natsrcpath);
        return(RH4N_RET_MALFORMED_FORMAT_STR);
    }

    strcpy(props->natlibrary, pointpos);
    strcpy(props->natprogram, formatstr);

    return(RH4N_RET_OK);
}

int rh4nnatDBOutputfileHandler(char *path, RH4nProperties *props) {
    if((props->outputfile = malloc((strlen(path)+1)*sizeof(char))) == NULL) {
        return(RH4N_RET_MEMORY_ERR);
    }

    strcpy(props->outputfile, path);
    return(RH4N_RET_OK);
}
