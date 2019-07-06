#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"

int rh4nldaGetArrayType(char *line, int *dimensions, int length[3], RH4nProperties *props, char *errorstr)
{
    *dimensions= 0;

    char *end_var_params = NULL, *comma = NULL,
         *double_collon = NULL;

    if((end_var_params = strchr(line, ')')) == NULL)
    {
        sprintf(errorstr, "Can not find array parms end");
        rh4n_log_error(props->logging, "%s", errorstr);
        return(RH4N_RET_LDA_PARSE_ERR);
    }
    *end_var_params = 0x00;

    comma = rh4nldastrtok(line, ',');
    while(comma)
    {
        (*dimensions)++;
        //Check if a lower bound is specified
        if((double_collon = strchr(comma, ':')) != NULL)
        {
            *double_collon = 0x00;
            double_collon++;
            length[(*dimensions)-1] = atoi(double_collon);
        }
        else
        {
            length[(*dimensions)-1] = atoi(comma);
        }
        comma = rh4nldastrtok(NULL, ',');
    }
    return(RH4N_RET_OK);
}
