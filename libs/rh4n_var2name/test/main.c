#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"
#include "rh4n_v2n.h"

int main(int argc, char *argv[]) {
    RH4nProperties props;
    char errorstr[2048];
    int parserret = 0;
    RH4nLDAEntry_t *entries = NULL;
    RH4nVarList natvars;
    RH4nVarPositions_t pos = {-1, NULL};

    props.logging = rh4nLoggingCreateRule("LDALIB", "LDAPARSE", RH4N_DEVELOP, "./logs/");
    //props.logging = rh4nLoggingCreateStreamingRule("LDALIB", "LDAPARSE", RH4N_DEVELOP, "./logs/");
    parserret = rh4nldaStartParser("./libs/rh4n_var2name/testfiles/TESTLDA.NGL", &entries, &props, errorstr);
    rh4n_log_debug(props.logging, "Parser ret: %d", parserret);
    rh4n_log_debug(props.logging, "Error: [%s]", errorstr);
    rh4n_log_debug(props.logging, "---------------------------------------------------------------");
    rh4nldaPrintList(entries, &props);

    rh4nvarInitList(&natvars);

    rh4nvarCreateNewString(&natvars, NULL, "str1", "val1");
    rh4nvarCreateNewString(&natvars, NULL, "str2", "val2");
    rh4nvarCreateNewString(&natvars, NULL, "str3", "val3");
    rh4nvarCreateNewString(&natvars, NULL, "str4", "val3");

    rh4nvarPrintList(&natvars, &props);

    rh4nv2nStart(&natvars, entries, "GRP1", &props, errorstr);

    rh4nvarPrintList(&natvars, &props);


    //rh4nv2nStartPosition(entries, "grpone", &pos, &props, errorstr);

    rh4nldaFreeList(entries);
}
