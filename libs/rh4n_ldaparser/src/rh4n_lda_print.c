#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"

#define RH4NLDA_PRINTTABS "\t\t\t\t\t\t\t\t\t\t"

void rh4nldaPrintList(RH4nLDAEntry_t *pldaentries, RH4nProperties *props) {
    rh4nldaPrintFork(pldaentries, props, 0);
}

void rh4nldaPrintFork(RH4nLDAEntry_t *anker, RH4nProperties *props, int level) {
    RH4nLDAEntry_t *hptr = NULL;

    for(hptr = anker; hptr != NULL; hptr=hptr->next) {
        rh4n_log_debug(props->logging, "%.*sLevel: %d", level, RH4NLDA_PRINTTABS, hptr->level);
        rh4n_log_debug(props->logging, "%.*sName: %s", level, RH4NLDA_PRINTTABS, hptr->name);

        if(hptr->type == RH4NLDA_SAG_GROUP_HEAD) rh4n_log_debug(props->logging, "%.*sType: Group", level, RH4NLDA_PRINTTABS);
        else if(hptr->type == RH4NLDA_SAG_REDEFINE) rh4n_log_debug(props->logging, "%.*sType: Redefine", level, RH4NLDA_PRINTTABS);
        else rh4n_log_debug(props->logging, "%.*sType: %c", level, RH4NLDA_PRINTTABS, hptr->type);

        if(hptr->length == -1) rh4n_log_debug(props->logging, "%.*sLength: Dynamic", level, RH4NLDA_PRINTTABS);
        else rh4n_log_debug(props->logging, "%.*sLength: %d", level, RH4NLDA_PRINTTABS, hptr->length);

        if(hptr->dimensions == 1) rh4n_log_debug(props->logging, "%.*sDimensions: 1, x: %d", level, RH4NLDA_PRINTTABS, hptr->arraylength[0]);
        else if(hptr->dimensions == 2) rh4n_log_debug(props->logging, "%.*sDimensions: 2, x: %d y: %d", level, RH4NLDA_PRINTTABS, hptr->arraylength[0], hptr->arraylength[1]);
        else if(hptr->dimensions == 3) rh4n_log_debug(props->logging, "%.*sDimensions: 3, x: %d y: %d z: %d", level, RH4NLDA_PRINTTABS, hptr->arraylength[0],
                                                                                                              hptr->arraylength[1], hptr->arraylength[2]);
        rh4n_log_debug(props->logging, "");

        if(hptr->nextlvl) rh4nldaPrintFork(hptr->nextlvl, props, level+1);
    }
}
