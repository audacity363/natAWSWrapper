#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"

int main(int argc, char *argv[]) {
    RH4nProperties props;
    char errorstr[2048];
    int parserret = 0;
    RH4nLDAEntry_t *entries = NULL;

    props.logging = rh4nLoggingCreateStreamingRule("LDALIB", "LDAPARSE", RH4N_DEVELOP, "");
    //parserret = rh4nldaStartParser("./testfiles/TEST.NGL", &entries, &props, errorstr);
    parserret = rh4nldaStartParser("./libs/rh4n_ldaparser/testfiles/RHLLREST.NGL", &entries, &props, errorstr);
    rh4n_log_debug(props.logging, "Parser ret: %d", parserret);
    rh4n_log_debug(props.logging, "Error: [%s]", errorstr);
    rh4n_log_debug(props.logging, "---------------------------------------------------------------");

    rh4nldaPrintList(entries, &props);
    rh4nldaFreeList(entries);
}
