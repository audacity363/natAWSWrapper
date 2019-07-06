#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"

int rh4nldaGetVarname(char **line, char *varname) {
    int varnamelength = 0;

    while((*line)[0] != ' ' && strlen(*line) > 0 && varnamelength <= NNI_LEN_NAME) {
        varname[varnamelength++] = (*line)[0];
        (*line)++;
    }
    varname[varnamelength] = 0x00;
    return(RH4N_RET_OK);
}

int rh4nldaCheckVartype(int type) {
    if(type == NNI_TYPE_BIN ||
       type == NNI_TYPE_CV  ||
       type == NNI_TYPE_OBJECT) 
        return(RH4N_LDA_VARTYPE_UNSUPPORTED);

    return(RH4N_RET_OK);
}

int rh4nldaGetVariablenLength(char **line, int *length, int type, char *errorstr)
{
    char var_length[RH4NLDA_MAX_LEN_CHAR+1];
    int i = 0;
    //Check if the Variable is from the length DYNAMIC
    if((*line)[0] == '/' || (*line)[0] == ')')
    {
        //Only Unicode and Alphanumeric can have a length of Dynamic
        //just trust on Natural that the Syntax is right
        if(type == NNI_TYPE_ALPHA || type == NNI_TYPE_UNICODE || type == NNI_TYPE_LOG)
        {
            *length = -1;
            return(RH4N_RET_OK);
        }
        //The rest of the variablen type don't have a length specification
        sprintf(errorstr, "Could not find variable length");
        return(RH4N_RET_LDA_PARSE_ERR);
    }
    for(i=0; ((*line)[0] != '/' && (*line)[0] != ')') && strlen(*line) > 0; i++) 
    {
        var_length[i] = (*line)[0];
        (*line)++;
    }
    var_length[i] = 0x00;

    *length = atoi(var_length);

    return(RH4N_RET_OK);
}
