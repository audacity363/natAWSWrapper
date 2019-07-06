#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"
#include "rh4n_v2n.h"
#include "rh4n_nat.h"
#include "rh4n_nat_varhandling.h"

struct RH4nPositionMatchingCallbacks {
    int vartype;
    int (*callback)(RH4nNatVarHandleParms*,char*,char*);
};

int rh4nnatPutParms(RH4nVarList *varlist, WORD nparms, void *parmhandle, RH4nVarPositions_t pos, RH4nNatInit_t *parms, 
        char *errorstr) {
    int i = 0, rc = 0, x = 0, callbacksize = 0;
    char *groupname = NULL;
    RH4nVarRef _refvar = RH4NVAR_REF_INIT;

    struct RH4nPositionMatchingCallbacks callbacks[] = {
        {NNI_TYPE_ALPHA, rh4nnatputString},
        {NNI_TYPE_FLOAT, rh4nnatputFloat},
        {NNI_TYPE_INT, rh4nnatputInteger},
        {NNI_TYPE_LOG, rh4nnatputBool},
        {NNI_TYPE_NUM, rh4nnatputAtoN}
    };
    struct parameter_description desc;
    RH4nNatVarHandleParms args = {
            parmhandle,
            0,
            NULL,
            NULL,
            parms->nnifuncs,
            varlist,
            parms->props,
            errorstr
        };

    callbacksize = sizeof(callbacks)/sizeof(struct RH4nPositionMatchingCallbacks);

    for(; i < pos.length; i++) {
        if((rc = parms->nnifuncs->pf_nni_get_parm_info(parms->nnifuncs, pos.parm_positions[i].position+2,
            parmhandle, &desc)) != NNI_RC_OK) {
            sprintf(errorstr, "Could not get parm description on parms %d. NNI return: %d", pos.parm_positions[i].position+2, rc);
            return(RH4N_RET_NNI_ERR);
        }

    
        if(strlen(pos.parm_positions[i].groupname) == 0) groupname = NULL;
        else groupname = pos.parm_positions[i].groupname;

        args.parmindex = pos.parm_positions[i].position+2;
        args.desc = &desc;

        rh4n_log_debug(parms->props->logging, "Natural var type: [%c]", desc.format);
        rh4n_log_debug(parms->props->logging, "Natural var size: [%d]", desc.length);

        if((rc = rh4nvarGetRef(varlist, groupname, pos.parm_positions[i].varname, &_refvar)) != RH4N_RET_OK) {
            rh4n_log_warn(parms->props->logging, "Could not get Variable: %s.%s. Varlib return: %d", groupname, 
                pos.parm_positions[i].varname, rc);
            continue;
        }
        
        for(x=0; x < callbacksize; x++)  {
            if(callbacks[x].vartype == desc.format) {
                if(((rc = callbacks[x].callback(&args, groupname,
                        pos.parm_positions[i].varname)) != RH4N_RET_OK) && rc != RH4N_RET_USE_F8) {
                    return(rc);
                }
            }
        }
    }
    return(RH4N_RET_OK);
}
