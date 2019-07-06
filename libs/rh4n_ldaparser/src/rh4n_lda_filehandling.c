#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"


int rh4nldaReadLine(FILE *fplda, char **ppline, RH4nProperties *props) {
    int length = 0;
    char *line = NULL, curchar = 0;

    if((line = malloc(1)) == NULL) return(RH4N_RET_MEMORY_ERR);

    while((curchar = fgetc(fplda)) != EOF) {
        switch(curchar) {
            case 0x00:
                line[length] = 0x00;
                *ppline = line;
                return(RH4N_RET_OK);
            case 0x1E: //Last char in File before EOF
                if(line != NULL) free(line);
                return(RH4N_LDA_BREAK);
            default:
                line[length++] = curchar;
                if((line = realloc(line, length+1)) == NULL) { 
                    rh4n_log_error(props->logging, "Could not allocate memory for lda line");
                    return(RH4N_RET_MEMORY_ERR);
                }
        }
    }
    return(RH4N_LDA_BREAK);
}
