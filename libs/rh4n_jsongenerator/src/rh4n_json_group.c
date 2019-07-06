#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>

#include "rh4n.h"
#include "rh4n_json.h"

bool rh4njsonCheckIfOnlyOneGroup(RH4nVarEntry_t *anker, RH4nProperties *props) {
    int groups_found = 0;
    if((groups_found = rh4njsonCountGroups(anker, props)) > 1) {
        return(false);
    }
    return(true);
}

int rh4njsonCountGroups(RH4nVarEntry_t *anker, RH4nProperties *props) {
    RH4nVarEntry_t *hptr = NULL;
    int groups_found = 0;

    for(hptr = anker; hptr != NULL; hptr = hptr->next) {
        if(hptr->var.type == RH4NVARTYPEGROUP) { 
            groups_found++;
        }
    }
    return(groups_found);
}
