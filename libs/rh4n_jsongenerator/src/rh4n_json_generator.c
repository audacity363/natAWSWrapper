#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <errno.h>

#include "rh4n.h"
#include "rh4n_json.h"

int rh4njsonPrintJSONToFile(RH4nVarList *varlist, char *filename, RH4nProperties *props) {
    FILE *outputfile = NULL;
    if((outputfile = fopen(filename, "w")) == NULL) {
        rh4n_log_error(props->logging, "Could not open [%s] => %s", filename, strerror(errno));
        return(RH4N_RET_INTERNAL_ERR);
    }
    rh4n_log_develop(props->logging, "Successfully opened [%s]", filename);
    rh4njsonPrintJSON(varlist, outputfile, props);
    fclose(outputfile);
    return(RH4N_RET_OK);
}

void rh4njsonPrintJSON(RH4nVarList *varlist, FILE *outputfile, RH4nProperties *props) {
    rh4njsonPrintObject(varlist->anker, outputfile, 0, props);
}

void rh4njsonPrintObject(RH4nVarEntry_t *anker, FILE *outputfile, int level, RH4nProperties *props) {
    RH4nVarEntry_t *hptr = NULL;
    int groups_found = 0;

    //if(rh4njsonCheckIfOnlyOneGroup(anker, props) == true) {
    if((groups_found = rh4njsonCountGroups(anker, props)) == 1) {
        if(rh4njsonCheckIfJSONArray(anker, props, 1) == true)   {
            rh4n_log_debug(props->logging, "Generating group [%s] as array", anker->name);
            rh4njsonPrintJSONObjectArray(anker, outputfile, props);
            return;
        }

        rh4n_log_debug(props->logging, "Das ist die Stelle!!!");

        //I have no idear why I have written this piece of code but i leave it in
        //in case i can remeber it....
#if 0
        rh4n_log_develop(props->logging, "Not only arrays in [%s]", anker->name);
        fprintf(outputfile, "}");
        fprintf(outputfile, "{");
        return;
#endif
    } else if(groups_found == 0) {
        rh4n_log_develop(props->logging, "Found no group in root");
        if(rh4njsonCheckIfJSONArray(anker, props, 0) == true) {
            rh4n_log_debug(props->logging, "All entries are arrays. Generate as object array");
            rh4njsonPrintJSONObjectArray(anker, outputfile, props);
            return;
        }
    } else {
        rh4n_log_develop(props->logging, "Found multiple Groups in root");
    }
    rh4n_log_debug(props->logging, "Multiple entries on level %d", level);

    rh4n_log_debug(props->logging, "Just generate a normal object");

    if(anker->prev != NULL || level == 0) fwprintf(outputfile, L"{");

    for(hptr = anker; hptr != NULL; hptr = hptr->next) {
        fwprintf(outputfile, L"\"%s\":", hptr->name);
        if(hptr->var.type == RH4NVARTYPEGROUP) {
            rh4njsonPrintObject(hptr->nextlvl, outputfile, level+1, props);
        } else if(hptr->var.type == RH4NVARTYPEARRAY) {
            rh4njsonPrintJSONArray(&hptr->var, outputfile, props);
        } else {
            rh4nvarPrintVar(&hptr->var, props, outputfile); 
        }
        if(hptr->next != NULL) { fwprintf(outputfile, L","); }
    }

    if(anker->prev != NULL || level == 0) fwprintf(outputfile, L"}");
}

