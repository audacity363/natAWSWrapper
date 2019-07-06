#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#include "rh4n.h"

int rh4nUtilsDumpProperties(char *filename, RH4nProperties *props) {
    int outputfile = -1 i_tmp = 0;
    char c_tmp = 0;

    if((outputfile = open(filename, O_WRONLY)) < 0) {
        return(RH4N_RET_FILE_ERR);
    }

    write(outputfile, props->natlibrary, sizeof(porps->natlibrary));
    write(outputfile, props->natprogram, sizeof(porps->natprogram));

    i_tmp = strlen(props->natparms)+1;
    write(outputfile, &i_tmp, sizeof(i_tmp));
    write(outputfile, props->natparms, i_tmp);
    
    write(outputfile, props->c_loglevel, sizeof(props->c_loglevel));
    write(outputfile, &props->i_loglevel, sizeof(props->i_loglevel));

    write(outputfile, props->httpreqtype, sizeof(props->httpreqtype));

    if(props->urlvars != NULL) {
        c_tmp = 1;
        write(outputfile, &c_tmp, sizeof(c_tmp));
        rh4nvarDumpAppendToFile(outputfile, &props->urlvars);
    } else {
        write(outputfile, &c_tmp, sizeof(c_tmp));
    }

    if(props->bodyvars != NULL) {
        c_tmp = 1;
        write(outputfile, &c_tmp, sizeof(c_tmp));
        rh4nvarDumpAppendToFile(outputfile, &props->bodyvars);
    } else {
        write(outputfile, &c_tmp, sizeof(c_tmp));
    }

    i_tmp = strlen(props->natsrcpath)+1;
    write(outputfile, &i_tmp, sizeof(i_tmp));
    write(outputfile, props->natsrcpath, i_tmp);

    i_tmp = strlen(props->outputfile)+1;
    write(outputfile, &i_tmp, sizeof(i_tmp));
    write(outputfile, props->outputfile, i_tmp);

    i_tmp = strlen(props->logpath)+1;
    write(outputfile, &i_tmp, sizeof(i_tmp));
    write(outputfile, props->logpath, i_tmp);

    write(outputfile, props->errorrepresentation, sizeof(props->errorrepresentation));
   
    i_tmp = strlen(props->username);
    write(outputfile, &i_tmp, sizeof(i_tmp));
    write(outputfile, props->username, i_tmp);
    
    close(outputfile);
    return(RH4N_RET_OK);
}

int rh4nUtilsLoadProperties(char *filename, RH4nProperties *props) {
    int inputfile = 0, ret = 0, i_tmp = 0;

    if((inputfile = open(filename, O_RDONLY)) < 0) {
        return(RH4N_RET_FILE_ERR);
    }

    memset(props, 0x00, sizeof(RH4nProperties));

    RH4N_UTILS_READ(inputfile, props->natlibrary, sizeof(props->natlibrary), ret);
    RH4N_UTILS_READ(inputfile, props->natprogram, sizeof(props->natprogram), ret);
    RH4N_UTILS_READ(inputfile, props->natprogram, sizeof(props->natprogram), ret);
    RH4N_UTILS_READ(inputfile, props->natprogram, sizeof(props->natprogram), ret);

    close(inputfile);
    return(RH4N_RET_OK);
}
