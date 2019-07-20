#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_logging.h"
#include "rh4n_utils.h"
#include "rh4n_natcall.h"

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <propertyFile>\n", argv[0]);
        fflush(stderr);
        exit(1);
    }

    int ret = 0;
	RH4nProperties props;

	if((ret = rh4nUtilsLoadProperties(argv[1], &props)) != RH4N_RET_OK) {
		exit(ret);
	}

	props.logging = rh4nLoggingCreateStreamingRule(props.natlibrary, props.natprogram, RH4N_DEVELOP, props.logpath);

	rh4nCallNatural(&props);


}
