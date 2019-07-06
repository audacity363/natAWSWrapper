#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"

int rh4nldaSaveEntry(RH4nLDAEntry_t *newentry, RH4nProperties *props) {
    RH4nLDAEntry_t *hptr = NULL;

    if(g_rh4nldaentries == NULL) {
        rh4n_log_debug(props->logging, "Saving first entry");
        g_rh4nldaentries = g_rh4nldacurptr = newentry;
        return(RH4N_RET_OK);
    }

    if(newentry->level == g_rh4nldacurptr->level) {
        rh4n_log_debug(props->logging, "Add entry on the same level");
        g_rh4nldacurptr->next = newentry;
        newentry->prev = g_rh4nldacurptr;
        g_rh4nldacurptr = newentry;
        return(RH4N_RET_OK);
    } else if(newentry->level > g_rh4nldacurptr->level) {
        rh4n_log_debug(props->logging, "Add entry on the next level");
        g_rh4nldacurptr->nextlvl = newentry;
        newentry->prev = g_rh4nldacurptr;
        g_rh4nldacurptr = newentry;
        return(RH4N_RET_OK);
    } else if(newentry->level < g_rh4nldacurptr->level) {
        rh4n_log_debug(props->logging, "Add entry above the curent entry");
        rh4n_log_debug(props->logging, "Old level: %d, New level: %d", g_rh4nldacurptr->level, newentry->level);
        for(hptr = g_rh4nldacurptr; hptr->level != newentry->level; hptr = hptr->prev);
        hptr->next = newentry;
        newentry->prev = hptr;
        g_rh4nldacurptr = newentry;
        return(RH4N_RET_OK);
    }
    return(RH4N_RET_OK);
}

char *savptr = NULL;
char *rh4nldastrtok(char *str, char delmiter)
{
    char *delmiter_pos = NULL, *tmp = NULL;

    if(str == NULL && savptr == NULL)
        return(NULL);

    if(str)
    {
        if((delmiter_pos = strchr(str, delmiter)) == NULL)
        {
            savptr = str+strlen(str);
            return(str);
        }
        *delmiter_pos = 0x00;
        savptr = ++delmiter_pos;
        return(str);
    }
    if(strlen(savptr) == 0)
    {
        savptr = NULL;
        return(NULL);
    }

    if((delmiter_pos = strchr(savptr, delmiter)) == NULL)    
    {
        tmp = savptr;
        savptr = savptr+strlen(savptr);
        return(tmp);
    }
    *delmiter_pos = 0x00;
    tmp = savptr;
    savptr = ++delmiter_pos;
    return(tmp);
}

void rh4nldaInitEntry(RH4nLDAEntry_t *entry) {
    entry->level = -1;
    entry->type = -1;
    memset(entry->name, 0x00, sizeof(entry->name));
    entry->dimensions = 0;
    entry->length = -1;
    memset(entry->arraylength, 0x00, sizeof(entry->length));
    entry->prev = NULL;
    entry->next = NULL;
    entry->nextlvl = NULL;
}
