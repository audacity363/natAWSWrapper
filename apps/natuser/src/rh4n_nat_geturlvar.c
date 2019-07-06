#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_v2n.h"
#include "rh4n_nat.h"
#include "rh4n_nat_varhandling.h"

long rh4nnatGetURLVar(WORD nparms, void *parmhandle, void *traditional) {
    if(traditional)  return(RH4N_RET_TARADITIONAL); 
    if(nparms != 3)  return(RH4N_RET_PARM_MISSMATCH);
    
    char errorstr[2048], groupname[100], varname[100];
    char *pgroupname = NULL;
    int utilsret = 0;
    RH4nNatInit_t initparms = {NULL, NULL, NULL, NULL};
    RH4nVarPositions_t pos = {-1, NULL };

    if((utilsret = rh4nnatInit(parmhandle, &initparms, errorstr, true)) != RH4N_RET_OK) {
        return(utilsret);
    }

    rh4n_log_debug(initparms.props->logging, "Sucessfully got default parms:");
    rh4n_log_debug(initparms.props->logging, "formatstr: [%s]", initparms.formatstr);
    parseVariableFormatStr(initparms.formatstr, initparms.props, groupname, varname);

    rh4n_log_info(initparms.props->logging, "God groupname: [%s]", groupname);
    rh4n_log_info(initparms.props->logging, "God varname:   [%s]", varname);

    if(strlen(groupname) == 0) pgroupname = NULL;
    else pgroupname = groupname;

    rh4nvarPrintList(&initparms.props->urlvars, initparms.props);

    if(rh4nvarExist(&initparms.props->urlvars, pgroupname, varname) == false) {
        rh4n_log_error(initparms.props->logging, "Could not find variable [%s].[%s]. varlib return: %d", 
            groupname, varname, utilsret);
        return(RH4N_RET_UNKOWN_VAR);
    }
    
    pos.length = 1;
    if((pos.parm_positions = malloc(sizeof(RH4nVarPosition_t))) == NULL) {
        free(initparms.formatstr);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        return(RH4N_RET_MEMORY_ERR);
    }

    strcpy(pos.parm_positions[0].groupname, groupname);
    strcpy(pos.parm_positions[0].varname, varname);
    pos.parm_positions[0].position = 0;

    if((utilsret = rh4nnatPutParms(&initparms.props->urlvars, nparms, parmhandle, pos, &initparms, errorstr)) != RH4N_RET_OK) {
        rh4n_log_error(initparms.props->logging, "Could not set vars: %d %s", utilsret, errorstr);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        free(initparms.formatstr);
        free(pos.parm_positions);
        return(utilsret);
    }

    free(pos.parm_positions);
    rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
    free(initparms.formatstr);
    return(RH4N_RET_OK);
}
