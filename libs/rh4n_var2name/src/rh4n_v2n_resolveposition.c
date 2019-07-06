#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"
#include "rh4n_v2n.h"

int rh4nv2nMatchPosition(RH4nLDAEntry_t  *ldalist, char *group, int *position, RH4nVarPositions_t *pos, 
                         RH4nProperties *props) {
    RH4nLDAEntry_t *hptr = ldalist;
    int v2nlibret = RH4N_RET_OK;

    for(; hptr; hptr=hptr->next) {
        if(hptr->type == RH4NLDA_SAG_REDEFINE) {
            rh4n_log_debug(props->logging, "Ignoring redefine [%s]", hptr->name);
            continue;
        } else if(hptr->type == RH4NLDA_SAG_GROUP_HEAD) {
            rh4n_log_debug(props->logging, "Found Group [%s]. Forking...", hptr->name);
            if((v2nlibret = rh4nv2nMatchPosition(hptr->nextlvl, hptr->name, position, pos, props)) != RH4N_RET_OK) {
                return(v2nlibret);
            }
        } else {
            if((v2nlibret = rh4nv2nSavePosition(hptr->name, group, *position, pos, props)) != RH4N_RET_OK) 
                return(v2nlibret);
            (*position)++; 
        }

    }

    return(RH4N_RET_OK);
}

int rh4nv2nSavePosition(char *name, char *group, int position, RH4nVarPositions_t *pos, RH4nProperties *props) {
    rh4n_log_debug(props->logging, "Saving Position");
    rh4n_log_debug(props->logging, "Group: %s", group == NULL ? "" : group);
    rh4n_log_debug(props->logging, "Name: %s", name);
    rh4n_log_debug(props->logging, "Position: %d\n", position);

    if(pos->length == -1) {
        if((pos->parm_positions = malloc(sizeof(RH4nVarPosition_t))) == NULL) return(RH4N_RET_MEMORY_ERR); 
        pos->length = 1;
    } else {
        if((pos->parm_positions = realloc(pos->parm_positions, 
            sizeof(RH4nVarPosition_t)*(++pos->length))) == NULL) return(RH4N_RET_MEMORY_ERR);
    }
   
    if(!group) {
        memset(pos->parm_positions[pos->length-1].groupname, 0x00, 
            sizeof(pos->parm_positions[pos->length-1].groupname));
    } else {
        strcpy(pos->parm_positions[pos->length-1].groupname, group);
    }

    strcpy(pos->parm_positions[pos->length-1].varname, name);
    pos->parm_positions[pos->length-1].position = position;
    return(RH4N_RET_OK);
}
