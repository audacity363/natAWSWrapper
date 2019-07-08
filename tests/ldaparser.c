#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("Usage: %s <path to LDA>\n", argv[0]);
        return(1);
    }

    RH4nLDAEntry_t *parsedLDA = NULL;
    char errorstr[1024];
    RH4nProperties props; memset(&props, 0x00, sizeof(props));

    props.logging = rh4nLoggingCreateStreamingRule("LIB", "PROG", RH4N_DEVELOP, "");

    rh4nldaStartParser(argv[1], &parsedLDA, &props, errorstr);
}
