#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_nat.h"
#include "rh4n_nat_varhandling.h"

int rh4nnatStartVariableReadout(WORD nparms, void *parmhandle, pnni_611_functions nnifuncs, 
                                RH4nVarList *varlist, RH4nProperties *props, char *errorstr) {
    int i = 0, x = 0, nniret = 0, callbacktabsize = 0, handleret = 0;
    struct parameter_description var_description;
    RH4nNatVarHandleParms args;
    char tmpvarname[100];

    args.parmhandle = parmhandle;
    args.nnifuncs = nnifuncs;
    args.props = props;
    args.errorstr = errorstr;
    args.varlist = varlist;

    struct RH4nNatVarHandling callbacks[] = {
        { NNI_TYPE_ALPHA, rh4nnatStringHandler },
        { NNI_TYPE_INT, rh4nnatIntegerHandler },
        { NNI_TYPE_LOG, rh4nnatBoolHandler },
        { NNI_TYPE_FLOAT, rh4nnatFloatHandler },
        { NNI_TYPE_DATE, rh4nnatDateTimeHandler },
        { NNI_TYPE_TIME, rh4nnatDateTimeHandler },
        { NNI_TYPE_NUM, rh4nnatNumericPackedHandler },
        { NNI_TYPE_UNICODE, rh4nnatUnicodeHandler }
        //Packed type does currently not work. You get a wrong string representation 
        //{ NNI_TYPE_PACK, rh4nnatNumericPackedHandler }

    };

    callbacktabsize = sizeof(callbacks)/sizeof(struct RH4nNatVarHandling);

    for(i = 2; i < nparms; i++) {
        memset(tmpvarname, 0x00, sizeof(tmpvarname));
        memset(&var_description, 0x00, sizeof(struct parameter_description));
        sprintf(tmpvarname, "var%d", i);
        
        if((nniret = nnifuncs->pf_nni_get_parm_info(nnifuncs, i, parmhandle, &var_description)) != NNI_RC_OK) {
            sprintf(errorstr, "Could not get parm %d. NNI error: %d", i, nniret);
            return(RH4N_RET_NNI_ERR);
        }

        args.parmindex = i;
        args.desc = &var_description;
        args.tmpname = tmpvarname;

        rh4n_log_debug(props->logging, "Got parameter infos:");
        rh4n_log_debug(props->logging, "Vartype: %c", var_description.format);
        rh4n_log_debug(props->logging, "Length: %d", var_description.length);
        rh4n_log_debug(props->logging, "Dimensions: %d", var_description.dimensions);
        rh4n_log_debug(props->logging, "Array length: X: %.2d, Y: %.2d, Z: %.2d", var_description.occurrences[0],
            var_description.occurrences[1], var_description.occurrences[2]);

        for(x = 0; x < callbacktabsize; x++) {
            if(callbacks[x].vartype == var_description.format) {
                if((handleret = callbacks[x].callback(&args)) != RH4N_RET_OK) return(handleret);
                break;
            }
        }
    }
    rh4nvarPrintList(varlist, props);
    return(RH4N_RET_OK);
}
