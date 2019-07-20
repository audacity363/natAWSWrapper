#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "rh4n.h"
#include "rh4n_natcall.h"

void rh4nlogInternalError(RH4nProperties *props, char *error_str) {
    FILE *outputfile = NULL;

    if((outputfile = fopen(props->outputfile, "w")) == NULL) {
        rh4n_log_error(props->logging, "Could not open outputfile: [%s]", props->outputfile);
        rh4n_log_error(props->logging, "Response will be empty");
        return;
    }

    if(strcmp(props->errorrepresentation, "JSON") == 0) {
        fprintf(outputfile, "{\"error\":true,\"type\":\"internal\",msg:\"%s\"}", error_str);
    } else {
        fprintf(outputfile, "<html><head><title>Internal Error</title></head><body><h2>%s</h2></body>", error_str);
    }

    fclose(outputfile);
    return;
}

void rh4nHandleNaturalError(RH4nProperties *props, int nniret, struct natural_exception natex) {
    char error_str[2048];

    rh4n_log_error(props->logging, "Error while executing natural program: %d", nniret);
    if(nniret > 0) {
        rh4n_log_error(props->logging, "Natural runtime error: %d", natex.natMessageNumber);
        rh4nlogNaturalError(props, natex);
        return;
    }

    sprintf(error_str, "Could not run Natural program due to a NNI error. NNIret: %d", nniret);
    rh4nlogInternalError(props, error_str);
    return;
}

void rh4nlogNaturalError(RH4nProperties *props, struct natural_exception natex) {
    FILE *outputfile = NULL;

    if((outputfile = fopen(props->outputfile, "w")) == NULL) {
        rh4n_log_error(props->logging, "Could not open outputfile: [%s]", props->outputfile);
        rh4n_log_error(props->logging, "Response will be empty");
        return;
    }


    if(strcmp(props->errorrepresentation, "JSON") == 0) {
        fprintf(outputfile, RH4N_TEMPLATE_NAT_JSON, natex.natMessageNumber, natex.natMessageText, 
            natex.natLibrary, natex.natMember, natex.natName, natex.natMethod, natex.natLine);
    } else {
        fprintf(outputfile, RH4N_TEMPLATE_NAT_HTML, natex.natMessageNumber, natex.natMessageNumber, natex.natMessageText, 
            natex.natLibrary, natex.natMember, natex.natName, natex.natMethod, natex.natLine);
    }

    fclose(outputfile);
    return;
}
