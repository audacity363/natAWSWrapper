#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"


void rh4nldaFreeList(RH4nLDAEntry_t *pldaentries) {
    rh4nldaFreeFork(pldaentries);
}

void rh4nldaFreeFork(RH4nLDAEntry_t *anker) {
    RH4nLDAEntry_t *hptr = NULL, *next = NULL;

    hptr = anker;
    while(hptr) {
        next = hptr->next;
        if(hptr->nextlvl) rh4nldaFreeFork(hptr->nextlvl);
        free(hptr);
        hptr = next;
    }
}
