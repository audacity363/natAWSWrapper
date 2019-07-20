#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_nat.h"


int rh4nnatGenerateDummyProperty(RH4nProperties **props);

int rh4nnatGetParameter(pnni_611_functions nnifuncs, void *parmhandle, RH4nProperties **props, char **formatstr, char *errorstr, bool checkformat) {
    struct parameter_description pdprop, pdformat;
    int nniret = 0;


    if((nniret = nnifuncs->pf_nni_get_parm_info(nnifuncs, RH4NNATPROPPOS, parmhandle, &pdprop)) != NNI_RC_OK) 
        return(RH4N_RET_NNI_ERR);
    if(pdprop.format != NNI_TYPE_BIN || pdprop.dimensions != 0 || pdprop.length_all == 0) {
        printf("Bin parm missmatch\n");
        return(RH4N_RET_PARM_MISSMATCH);
    }
    if((nniret = nnifuncs->pf_nni_get_parm(nnifuncs, RH4NNATPROPPOS, parmhandle, sizeof(RH4nProperties*), props)) != NNI_RC_OK) 
    {
        printf("nniret: %d\n", nniret);
        return(RH4N_RET_NNI_ERR);
    }
    if(*props == NULL) return(RH4N_RET_PARM_MISSMATCH);


    //rh4nnatGenerateDummyProperty(props);


    if(!checkformat) return(RH4N_RET_OK);

    if((nniret = nnifuncs->pf_nni_get_parm_info(nnifuncs, RH4NNATFORMATPOS, parmhandle, &pdformat)) != NNI_RC_OK) 
        return(RH4N_RET_NNI_ERR);
    if(pdformat.format != NNI_TYPE_ALPHA || pdformat.dimensions != 0 || pdformat.length_all == 0) {
        printf("Format A missmatch\n");
        return(RH4N_RET_PARM_MISSMATCH);
    }
    if((*formatstr = malloc(sizeof(char)*(pdformat.length_all+1))) == NULL) return(RH4N_RET_MEMORY_ERR);
    memset(*formatstr, 0x00, sizeof(char)*(pdformat.length_all+1));
    if((nniret = nnifuncs->pf_nni_get_parm(nnifuncs, RH4NNATFORMATPOS, parmhandle, pdformat.length_all, *formatstr)) != NNI_RC_OK) {
        free(*formatstr);
        return(RH4N_RET_NNI_ERR);
    }
    return(RH4N_RET_OK);
}

int rh4nnatParseFormatStr(char *formatstr, struct RH4nNatLDAInfos *pldainfos, RH4nProperties *props) {
    int i = 0;
    char *at_pos = NULL, *point_pos = NULL;

    for(; i < strlen(formatstr); i++) {
        if(formatstr[i] == '.' && at_pos == NULL) {
            //Found a point before a '@' char
            return(RH4N_RET_MALFORMED_FORMAT_STR);
        } else if(formatstr[i] == '.' && point_pos != NULL) {
            //Found second point
            return(RH4N_RET_MALFORMED_FORMAT_STR);
        } else if(formatstr[i] == '@' && at_pos != NULL) {
            //Found a second @
            return(RH4N_RET_MALFORMED_FORMAT_STR);
        } else if(formatstr[i] == '.') {
            point_pos = &formatstr[i];
        } else if(formatstr[i] == '@') {
            at_pos = &formatstr[i];
        }
    }

    if(at_pos == NULL) {
        //Didn't found an '@' char
        return(RH4N_RET_MALFORMED_FORMAT_STR);
    } else if(at_pos == formatstr) {
        //struct name is empty
        printf("Struct name ist empty\n"); fflush(stdout);
        return(RH4N_RET_MALFORMED_FORMAT_STR);
    } 

    *at_pos = '\0';
    if(strlen(formatstr) > NNI_LEN_NAME) {
        //struct name to long
        return(RH4N_RET_BUFFER_OVERFLOW);
    }

    strcpy(pldainfos->struct_name, formatstr);

    if(point_pos) {
        if(point_pos == at_pos+1) {
            //library entry is empty
            printf("library Name is empty"); fflush(stdout);
            return(RH4N_RET_MALFORMED_FORMAT_STR);
        }
        *point_pos = '\0';
        if(strlen(at_pos+1) > NNI_LEN_LIBRARY) {
            //lib name is to long
            return(RH4N_RET_BUFFER_OVERFLOW);
        }

        strcpy(pldainfos->library, at_pos+1);

        if(strlen(point_pos+1) == 0) {
            //LDA name is empty
            printf("LDA Name is empty"); fflush(stdout);
            return(RH4N_RET_MALFORMED_FORMAT_STR);
        } else if(strlen(point_pos+1) > NNI_LEN_MEMBER) {
            //LDA name is to long
            return(RH4N_RET_BUFFER_OVERFLOW);
        }

        strcpy(pldainfos->ldaname, point_pos+1);
    } else {
        if(strlen(at_pos+1) == 0) {
            //LDA name is emptty
            printf("LDA Name is empty"); fflush(stdout);
            return(RH4N_RET_MALFORMED_FORMAT_STR);
        } else if(strlen(at_pos+1) > NNI_LEN_MEMBER) {
            //LDA name is to long
            return(RH4N_RET_BUFFER_OVERFLOW);
        }
        strcpy(pldainfos->ldaname, at_pos+1);

        strcpy(pldainfos->library, props->natlibrary);
    } 

    rh4n_log_debug(props->logging, "json-struct: [%s]", pldainfos->struct_name);
    rh4n_log_debug(props->logging, "lib: [%s]", pldainfos->library);
    rh4n_log_debug(props->logging, "lda: [%s]", pldainfos->ldaname);
    return(RH4N_RET_OK);
}

int parseVariableFormatStr(char *formatstr, RH4nProperties *props, char *groupname, char *varname) {
    char *pointpos = NULL;

    if((pointpos = strchr(formatstr, '.')) == NULL) {
        strcpy(varname, formatstr);
        *groupname = '\0';
    } else {
        *pointpos = '\0';
        strcpy(groupname, formatstr);
        strcpy(varname, pointpos+1);
    }

    rh4n_log_debug(props->logging, "Found group: [%s]", groupname);
    rh4n_log_debug(props->logging, "Found varname [%s]", varname);

    return(RH4N_RET_OK);
}

//Just a dummy function for the first aws lambda prototype
int rh4nnatGenerateDummyProperty(RH4nProperties **props) {
    static RH4nProperties lprops; memset(&lprops, 0x00, sizeof(props));

    lprops.natsrcpath = "/var/task/fuser";
    lprops.outputfile = "/tmp/test";
    
    lprops.logging = rh4nLoggingCreateStreamingRule("LIB", "PROG", RH4N_INFO, "");
    *props = &lprops;
    return(0);
}
