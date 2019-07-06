#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"
#include "rh4n_v2n.h"

int rh4nv2nStart(RH4nVarList *natanker, RH4nLDAEntry_t *ldaanker, char *structname, RH4nProperties *props, char *errorstr) {
    RH4nLDAEntry_t *targetstruct = NULL;
    int v2nerror = RH4N_RET_OK;

    if(rh4nv2nSearchTargetStruct(ldaanker, structname, &targetstruct, errorstr) != RH4N_RET_OK) {
        rh4n_log_error(props->logging, "%s", errorstr);
        return(RH4N_RET_STRUCT_ERR);
    }

    rh4n_log_debug(props->logging, "Found struct [%s]", structname);
    if(rh4nv2ncmpVarNumbers(natanker, targetstruct->nextlvl, props, errorstr) != RH4N_RET_OK) {
        rh4n_log_error(props->logging, "%s", errorstr);
        return(RH4N_RET_LDA_NAT_MISSMATCH);
    }

    rh4n_log_debug(props->logging, "Start resolving names");
    parents.length = 1;
    if((parents.groups = malloc(sizeof(char*))) == NULL) {
        return(RH4N_RET_MEMORY_ERR);
    }
    parents.groups[0] = NULL;
    rh4nv2nMatchNames(natanker->anker, targetstruct->nextlvl, natanker, NULL, props, &v2nerror);
    if(v2nerror != RH4N_RET_OK) {
        free(parents.groups);
        rh4n_log_error(props->logging, "Error while resolving names: %d", v2nerror);
        return(v2nerror);
    }
    free(parents.groups);
    
    rh4n_log_debug(props->logging, "Resolving names was successfully");
    return(RH4N_RET_OK);
}

int rh4nv2nStartPosition(RH4nLDAEntry_t *ldaanker, char *structname, RH4nVarPositions_t *pos, 
                         RH4nProperties *props, char *errorstr) {

    RH4nLDAEntry_t *targetstruct = NULL;
    int v2nlibret = RH4N_RET_OK, position = 0;

    if(rh4nv2nSearchTargetStruct(ldaanker, structname, &targetstruct, errorstr) != RH4N_RET_OK) {
        rh4n_log_error(props->logging, "%s", errorstr);
        return(RH4N_RET_STRUCT_ERR);
    }

    rh4n_log_debug(props->logging, "Found struct [%s]", structname);
    if((v2nlibret = rh4nv2nMatchPosition(targetstruct->nextlvl, NULL, &position, pos, props)) != RH4N_RET_OK) 
        return(v2nlibret);
    return(RH4N_RET_OK);
}

int rh4nv2ncmpVarNumbers(RH4nVarList *natanker, RH4nLDAEntry_t *ldaanker, RH4nProperties *props, char *errorstr)  {
    RH4nVarEntry_t *nathptr = NULL;
    int natvarnumber = 0, ldavarnumber = 0;

    //Count variables from natural
    for(nathptr = natanker->anker; nathptr; nathptr=nathptr->next) { natvarnumber++; }
    rh4n_log_debug(props->logging, "Found %d variables in natural anker", natvarnumber);

    //Count variables from the LDA
    ldavarnumber = rh4nv2ncountVarsinLDAGroup(ldaanker);
    rh4n_log_debug(props->logging, "Found %d variables in LDA anker", ldavarnumber);

    if(natvarnumber != ldavarnumber) { 
        sprintf(errorstr, "Variables does not match: Natural: %d LDA: %d", natvarnumber, ldavarnumber);
        return(RH4N_RET_LDA_NAT_MISSMATCH);
    }
    return(RH4N_RET_OK);
}

int rh4nv2ncountVarsinLDAGroup(RH4nLDAEntry_t *anker) {
    int numberofvars = 0;
    RH4nLDAEntry_t *hptr = NULL;

    for(hptr = anker; hptr; hptr=hptr->next) { 
        if(hptr->type == RH4NLDA_SAG_REDEFINE) continue;
        else if(hptr->type == RH4NLDA_SAG_GROUP_HEAD) numberofvars += rh4nv2ncountVarsinLDAGroup(hptr->nextlvl);
        else numberofvars++;
    }

    return(numberofvars);
}

int rh4nv2nSearchTargetStruct(RH4nLDAEntry_t *ldaanker, char *name, RH4nLDAEntry_t **target, char *errorstr) {
    RH4nLDAEntry_t *hptr = NULL;

    for(hptr = ldaanker; hptr; hptr=hptr->next) {
        if(strcmp(hptr->name, name) == 0) {
            if(hptr->type != RH4NLDA_SAG_GROUP_HEAD) { 
                sprintf(errorstr, "Struct [%s] is not a group head", name);
                return(RH4N_RET_STRUCT_ERR);
            }
            *target = hptr;
            return(RH4N_RET_OK);
        }
    }
    sprintf(errorstr, "Did not found struct [%s]", name);
    return(RH4N_RET_STRUCT_ERR);
} 
