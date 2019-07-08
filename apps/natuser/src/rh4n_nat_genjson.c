#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_utils.h"
#include "rh4n_nat.h"
#include "rh4n_ldaparser.h"
#include "rh4n_v2n.h"
#include "rh4n_json.h"

long rh4nnatGenJSON(WORD nparms, void *parmhandle, void *traditional) {
    if(traditional)  return(RH4N_RET_TARADITIONAL); 
    if(nparms <= 2)  return(RH4N_RET_PARM_MISSMATCH);

    char errorstr[2048], ldapath[2048];
    int utilsret = 0;
    RH4nNatInit_t initparms = {NULL, NULL, NULL, NULL};
    struct RH4nNatLDAInfos ldainfos;
    RH4nVarList varlist;
    RH4nLDAEntry_t *ldavars = NULL;

    //setlocale(LC_ALL, "de_DE");

    if((utilsret = rh4nnatInit(parmhandle, &initparms, errorstr, true)) != RH4N_RET_OK) {
        return(utilsret);
    }

    //rh4n_log_debug(initparms.props->logging, "Sucessfully got default parms:"); fflush(stderr);
    fprintf(stdout, "Sucessfully got default parms:\n");
    //rh4n_log_debug(initparms.props->logging, "formatstr: [%s]", initparms.formatstr); fflush(stderr);
    fprintf(stdout, "formatstr: [%s]\n", initparms.formatstr); fflush(stdout);

    //rh4n_log_debug(initparms.props->logging, "Start parsing formatstr");
    fprintf(stdout, "Start parsing formatstr\n"); fflush(stdout);
    if((utilsret = rh4nnatParseFormatStr(initparms.formatstr, &ldainfos, initparms.props)) != RH4N_RET_OK) {
        fprintf(stdout, "Error parsing format str\n"); fflush(stdout);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        free(initparms.formatstr);
        return(utilsret);
    }
    rh4n_log_info(initparms.props->logging, "Got following LDA infos:");
    rh4n_log_info(initparms.props->logging, "Struct name: [%s]", ldainfos.struct_name);
    rh4n_log_info(initparms.props->logging, "Library: [%s]", ldainfos.library);
    rh4n_log_info(initparms.props->logging, "LDA: [%s]", ldainfos.ldaname);

    //For the first prototype the library as to be passed over the format str
    if(strlen(ldainfos.library) == 0)  {
        printf("library is empty\n"); fflush(stdout);
        return(RH4N_RET_MALFORMED_FORMAT_STR);
    }

    rh4nvarInitList(&varlist);

    rh4n_log_info(initparms.props->logging, "Start dumping variables");
    if((utilsret = rh4nnatStartVariableReadout(nparms, parmhandle, initparms.nnifuncs, 
            &varlist, initparms.props, errorstr)) != RH4N_RET_OK) {
        rh4n_log_error(initparms.props->logging, "Error while reading variables: %d %s", utilsret, errorstr);
        printf("Error while reading variables: %d %s", utilsret, errorstr); fflush(stdout);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        free(initparms.formatstr);
        return(utilsret);
    }
    sprintf(ldapath, "%s/%s/GP/%s.NGL", initparms.props->natsrcpath, ldainfos.library, ldainfos.ldaname);
    rh4n_log_info(initparms.props->logging, "Start parsing LDA [%s]", ldapath);
    if((utilsret = rh4nldaStartParser(ldapath, &ldavars, initparms.props, errorstr)) != RH4N_RET_OK) {
        rh4n_log_error(initparms.props->logging, "Error while parsing LDA: %d %s", utilsret, errorstr);
        printf("Error while parsing LDA: %d %s", utilsret, errorstr); fflush(stdout);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        free(initparms.formatstr);
        return(utilsret);
    }
    rh4n_log_info(initparms.props->logging, "Successfully parsed LDA");
    rh4nldaPrintList(ldavars, initparms.props);
    rh4n_log_info(initparms.props->logging, "Start matching names");
    if((utilsret = rh4nv2nStart(&varlist, ldavars, ldainfos.struct_name, initparms.props, errorstr)) != RH4N_RET_OK) {
        rh4n_log_error(initparms.props->logging, "Could not match names: %d %s", utilsret, errorstr);
        printf("Could not match names: %d %s", utilsret, errorstr); fflush(stdout);
        rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
        rh4nldaFreeList(ldavars);
        free(initparms.formatstr);
        return(utilsret);
    }
    rh4n_log_info(initparms.props->logging, "Successfully matched names");

    //rh4nvarPrintJSONToFile(&varlist, initparms.props->outputfile, initparms.props);
    rh4n_log_info(initparms.props->logging, "Start generating JSON");
    RH4nVarList tmp;
    //tmp.anker = varlist.anker->nextlvl;
    tmp.anker = varlist.anker;
    rh4n_log_develop(initparms.props->logging, "Generate JSON from anker: [%s]->[%p]", tmp.anker->name, tmp.anker->name);
    rh4nvarPrintList(&tmp, initparms.props);
    rh4njsonPrintJSONToFile(&tmp, initparms.props->outputfile, initparms.props);

    rh4nUtilscloseSharedLibrary(initparms.sharedlibrary);
    rh4nldaFreeList(ldavars);
    free(initparms.formatstr);

    return(RH4N_RET_OK);
}
