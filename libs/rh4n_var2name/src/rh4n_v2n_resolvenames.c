#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"
#include "rh4n_v2n.h"

RH4nParentGroups_t parents;

RH4nVarEntry_t *rh4nv2nMatchNames(RH4nVarEntry_t *nat_set, RH4nLDAEntry_t *lda_set, RH4nVarList *varanker, char *groupname, RH4nProperties *props, int *error) {
    RH4nVarEntry_t *cur_nat_set = nat_set, *tmp = NULL;
    RH4nLDAEntry_t *cur_lda_set = lda_set;
    int varlibret = 0;
    
    if(groupname) {
        if(rh4nvarGroupExist_m(varanker, parents.groups, groupname) == 0) {
        rh4n_log_debug(props->logging, "Creating group [%s]", groupname);
            if((varlibret = rh4nvarCreateNewGroup_m(varanker, parents.groups, groupname)) != RH4N_RET_OK) { 
                rh4n_log_error(props->logging, "Could not create group %s. Varlib error: %d", groupname, varlibret);
                *error = varlibret; 
                return(NULL); 
            }
        }
            
        parents.length++;
        if((parents.groups = realloc(parents.groups, sizeof(char*)*parents.length)) == NULL) {
            *error = RH4N_RET_MEMORY_ERR;
            return(NULL);
        }
        if((parents.groups[parents.length-2] = malloc(strlen(groupname))) == NULL) {
            *error = RH4N_RET_MEMORY_ERR;
            return(NULL);
        }
        strcpy(parents.groups[parents.length-2], groupname);
        parents.groups[parents.length-1] = NULL;

    }

    for(; cur_lda_set; cur_lda_set=cur_lda_set->next) {
        if(cur_lda_set->type == RH4NLDA_SAG_REDEFINE) {
            rh4n_log_debug(props->logging, "Ignoring redefine [%s]", cur_lda_set->name);
            continue;
        } else if(cur_lda_set->type == RH4NLDA_SAG_GROUP_HEAD) {
            rh4n_log_debug(props->logging, "Found a group. Forking....  [%s]", cur_lda_set->name);
            cur_nat_set = rh4nv2nMatchNames(cur_nat_set, cur_lda_set->nextlvl, varanker, cur_lda_set->name, props, error);
            if(cur_nat_set == NULL) return(NULL);
        } else {
            rh4n_log_debug(props->logging, "Renaming [%s] to [%s]", cur_nat_set->name, cur_lda_set->name);
            free(cur_nat_set->name);
            if((cur_nat_set->name = malloc((strlen(cur_lda_set->name)+1)*sizeof(char))) == NULL) { 
                *error = RH4N_RET_MEMORY_ERR; 
                return(NULL); 
            }
            strcpy(cur_nat_set->name, cur_lda_set->name);

            tmp = cur_nat_set->next;
            if(parents.length > 1) {
                rh4n_log_debug(props->logging, "Moving [%s] to group [%s]", cur_nat_set->name, parents.groups[parents.length-2]);
                if((varlibret = rh4nvarMoveVarToGroup_m(varanker, cur_nat_set->name, parents.groups)) != RH4N_RET_OK) { 
                    rh4n_log_error(props->logging, "Error while moving %s to group %s", cur_nat_set->name, groupname);
                    *error = varlibret; 
                    return(NULL); 
                }
            }
            cur_nat_set = tmp;
        }
    }
    rh4n_log_debug(props->logging, "Arrived at end of list");
    if(groupname) {
        parents.length--;
        if((parents.groups = realloc(parents.groups, sizeof(char*)*parents.length)) == NULL) {
            *error = RH4N_RET_MEMORY_ERR;
            return(NULL);
        }
        free(parents.groups[parents.length-1]);
        parents.groups[parents.length-1] = NULL;
    }
    return(cur_nat_set);
}
