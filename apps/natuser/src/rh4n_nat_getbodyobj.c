#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_ldaparser.h"
#include "rh4n_v2n.h"
#include "rh4n_nat.h"
#include "rh4n_nat_varhandling.h"

long rh4nnatGetBodyObj(WORD nparms, void *parmhandle, void *traditional) {
    if(traditional)  return(RH4N_RET_TARADITIONAL); 
    if(nparms <= 2)  return(RH4N_RET_PARM_MISSMATCH);

    char errorstr[2048], ldapath[2048];
    int utilsret = 0;
    RH4nNatInit_t initparms = {NULL, NULL, NULL, NULL};
    struct RH4nNatLDAInfos ldainfos;
    RH4nLDAEntry_t *ldavars = NULL;
    RH4nVarPositions_t pos = {-1, NULL };

    if((utilsret = rh4nnatInit(parmhandle, &initparms, errorstr, true)) != RH4N_RET_OK) {
        return(utilsret);
    }

    rh4n_log_debug(initparms.props->logging, "Sucessfully got default parms:");
    rh4n_log_debug(initparms.props->logging, "formatstr: [%s]", initparms.formatstr);

    rh4n_log_debug(initparms.props->logging, "Start parsing formatstr");
    if((utilsret = rh4nnatParseFormatStr(initparms.formatstr, &ldainfos, initparms.props)) != RH4N_RET_OK) {
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        free(initparms.formatstr);
        return(utilsret);
    }
    rh4n_log_info(initparms.props->logging, "Got following LDA infos:");
    rh4n_log_info(initparms.props->logging, "Struct name: [%s]", ldainfos.struct_name);
    rh4n_log_info(initparms.props->logging, "Library: [%s]", ldainfos.library);
    rh4n_log_info(initparms.props->logging, "LDA: [%s]", ldainfos.ldaname);

    sprintf(ldapath, "%s/%s/GP/%s.NGL", initparms.props->natsrcpath, ldainfos.library, ldainfos.ldaname);
    rh4n_log_info(initparms.props->logging, "Start parsing LDA [%s]", ldapath);
    if((utilsret = rh4nldaStartParser(ldapath, &ldavars, initparms.props, errorstr)) != RH4N_RET_OK) {
        rh4n_log_error(initparms.props->logging, "Error while parsing LDA: %d %s", utilsret, errorstr);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        free(initparms.formatstr);
        return(utilsret);
    }
    rh4n_log_info(initparms.props->logging, "Successfully parsed LDA");
    rh4nldaPrintList(ldavars, initparms.props);

    if((utilsret = rh4nv2nStartPosition(ldavars, ldainfos.struct_name, &pos, initparms.props, errorstr)) != RH4N_RET_OK) {
        rh4n_log_error(initparms.props->logging, "Could not match positions: %d %s", utilsret, errorstr);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        rh4nldaFreeList(ldavars);
        free(initparms.formatstr);
        return(utilsret);
    }

    if((utilsret = rh4nnatPutParms(&initparms.props->bodyvars, nparms, parmhandle, pos, &initparms, errorstr)) != RH4N_RET_OK) {
        rh4n_log_error(initparms.props->logging, "Could not set vars: %d %s", utilsret, errorstr);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        rh4nldaFreeList(ldavars);
        free(initparms.formatstr);
        return(utilsret);
    }

    rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
    rh4nldaFreeList(ldavars);
    free(initparms.formatstr);
    return(RH4N_RET_OK);
}
