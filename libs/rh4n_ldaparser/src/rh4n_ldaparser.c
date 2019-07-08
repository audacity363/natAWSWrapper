#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>

#include "rh4n.h"
#include "rh4n_ldaparser.h"

RH4nLDAEntry_t *g_rh4nldaentries = NULL, *g_rh4nldacurptr = NULL;
int rh4nldaStartParser(char *ldapath, RH4nLDAEntry_t **ldaentries, RH4nProperties *props, char *errorstr) {
    FILE *fplda = NULL;
    char ldaheader[RH4N_LDA_HEADERSIZE], *ldaline = NULL;
    int fhret = 0, parseret = 0;
    
    if((fplda = fopen(ldapath, "r")) == NULL) {
        rh4n_log_error(props->logging, "Could not open LDA [%s]", ldapath);
        sprintf(errorstr, "Could not open LDA [%s]. Error: [%s]", ldapath, strerror(errno));
        return(RH4N_RET_LDA_PARSE_ERR);
    }

    //Jump over LDA header
    fread(ldaheader, RH4N_LDA_HEADERSIZE, 1, fplda);
   
    while((fhret = rh4nldaReadLine(fplda, &ldaline, props)) == RH4N_RET_OK) {
        rh4n_log_debug(props->logging, "LDA line: [%s]", ldaline);
        if((parseret = rh4nldaParseLine(ldaline, ldaentries, props, errorstr)) != RH4N_RET_OK) {
            rh4n_log_error(props->logging, "Error while parsing line");
            break;
        }
        free(ldaline);
        ldaline = NULL;
    }

    if(ldaline) { free(ldaline); }
    fclose(fplda);

    *ldaentries = g_rh4nldaentries;
    g_rh4nldacurptr = g_rh4nldaentries = NULL; 
    return(parseret);
}


int rh4nldaParseLine(char *line, RH4nLDAEntry_t **ldaentries, RH4nProperties *props, char *errorstr) {
    RH4nLDAEntry_t *curentry = NULL;
    char *_line = line, varname[NNI_LEN_NAME+1];

    if(RH4NLDA_CHECK_IF_VIEW(_line)) { 
        sprintf(errorstr, "View are not allorw in the LDA");
        return(RH4N_RET_LDA_PARSE_ERR); 
    } 
    else if(RH4NLDA_CHECK_IF_CONST(_line)) return(RH4N_RET_OK);
    else if(RH4NLDA_CHECK_IF_INIT(_line)) return(RH4N_RET_OK);

    if((curentry = malloc(sizeof(RH4nLDAEntry_t))) == NULL) return(RH4N_RET_MEMORY_ERR);
    rh4nldaInitEntry(curentry);
    
    _line += 5; //Jump over line number
    if(RH4NLDA_COMPARE_HEAD_STR(_line)) return(RH4N_RET_OK);
    if(RH4NLDA_COMPARE_FOOT_STR(_line)) return(RH4N_RET_OK);

    for(;*_line == ' '; _line++);

    //init value
    if(*_line == '<') return(RH4N_RET_OK);
    //array init value
    else if(*_line == '(') return(RH4N_RET_OK);
    //comment
    else if(*_line == '*') return(RH4N_RET_OK);

    //Convert char level to integer
    curentry->level = _line[0] - 0x30;

    rh4n_log_debug(props->logging, "Found level %d", curentry->level);
    _line += 2;

    rh4nldaGetVarname(&_line, varname);
    rh4n_log_debug(props->logging, "Found varname: [%s]", varname);
    strcpy(curentry->name, varname);

    for(;*_line == ' ' && strlen(_line) > 0; _line++);

    //Group without array
    if(strlen(_line) == 0) {
        rh4n_log_debug(props->logging, "Found Group");
        curentry->type = RH4NLDA_SAG_GROUP_HEAD;
        rh4nldaSaveEntry(curentry, props);
        return(RH4N_RET_OK);
    } else if(RH4NLDA_COMPARE_REDEFINE_STR(varname)) {
        rh4n_log_debug(props->logging, "Found a redefine");
        memset(varname, 0x00, sizeof(varname));

        rh4nldaGetVarname(&_line, varname);
        rh4n_log_debug(props->logging, "Name of redefined variable: [%s]", varname);

        strcpy(curentry->name, varname);
        curentry->type = RH4NLDA_SAG_REDEFINE;
        rh4nldaSaveEntry(curentry, props);
        return(RH4N_RET_OK);
    }
    
    if(*_line != '(') {
        sprintf(errorstr, "Unexpected char [%c] after varname", *_line);
        rh4n_log_error(props->logging, "%s", errorstr);
        free(curentry);
        return(RH4N_RET_LDA_PARSE_ERR);
    }

    _line++;

    //Group with array
    if(isdigit(*_line)) {
        rh4n_log_debug(props->logging, "Found Group with array definition");
        if(rh4nldaGetArrayType(_line, &curentry->dimensions, curentry->arraylength, props, errorstr) != RH4N_RET_OK) {
            return(RH4N_RET_LDA_PARSE_ERR);
        }
        rh4n_log_debug(props->logging, "Array specifications: dims: %d, x: %d y: %d, z: %d", 
            curentry->dimensions, curentry->arraylength[0],
            curentry->arraylength[1], curentry->arraylength[2]);

        curentry->type = RH4NLDA_SAG_GROUP_HEAD;
        rh4nldaSaveEntry(curentry, props);
        return(RH4N_RET_OK);
    }


    curentry->type = *_line;
    if(rh4nldaCheckVartype(curentry->type) == RH4N_LDA_VARTYPE_UNSUPPORTED) {
        sprintf(errorstr, "Vartype %c is currently not supported", curentry->type);
        rh4n_log_error(props->logging, "%s", errorstr);
        free(curentry);
        return(RH4N_RET_LDA_PARSE_ERR);
    }
    rh4n_log_debug(props->logging, "Found vartype: %c", curentry->type);

    _line++;
    if(rh4nldaGetVariablenLength(&_line, &curentry->length, curentry->type, errorstr) != RH4N_RET_OK) {
        rh4n_log_error(props->logging, "%s", errorstr);
        free(curentry);
        return(RH4N_RET_LDA_PARSE_ERR);
    }

    rh4n_log_debug(props->logging, "Found variable length: %d", curentry->length);

    if(*_line == '/') {
        if(rh4nldaGetArrayType(_line, &curentry->dimensions, curentry->arraylength, props, errorstr) != RH4N_RET_OK) {
            return(RH4N_RET_LDA_PARSE_ERR);
        }
        rh4n_log_debug(props->logging, "Found array specifications: dims: %d, x: %d y: %d, z: %d", 
            curentry->dimensions, curentry->arraylength[0],
            curentry->arraylength[1], curentry->arraylength[2]);
    }
    
    rh4nldaSaveEntry(curentry, props);
    return(RH4N_RET_OK);
}
