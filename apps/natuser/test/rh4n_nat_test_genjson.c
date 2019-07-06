#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "rh4n.h"
#include "rh4n_utils.h"

struct parameter_description* genjsontestParmGen(pnni_611_functions, RH4nProperties*);

int main(int argc, char *argv[]) {
    void *shnatlib = NULL;
    pnni_611_functions nnifuncs = NULL;
    char errorstr[2048];
    int utilsret = 0, nniret = 0, length[3] = { 3, -1, -1 };
    struct parameter_description *naturalparms = NULL;
    RH4nProperties props;
    struct natural_exception naturalex;
    RH4nVarList urlvars;

    strcpy(props.natlibrary, "TGAP0734");
    strcpy(props.natprogram, "RH4NTEST");
    props.natparms = "etid=$$";
    strcpy(props.c_loglevel, "DEVELOP");
    strcpy(props.httpreqtype, "GET");
    props.natsrcpath = "/VAW/natural/bonn/fuser63/";
    props.outputfile = "./output.html";
    props.logpath =  "./logs/";


    if((props.i_loglevel = rh4nLoggingConvertStrtoInt(props.c_loglevel)) < 0) {
        return(RH4N_RET_LOGGING_ERR);
    }


    if((props.logging = rh4nLoggingCreateStreamingRule(props.natlibrary, props.natprogram, props.i_loglevel, props.logpath)) == NULL) {
        printf("Could not create  Loggingrule\n");
        return(1);
    }

    rh4n_log_debug(props.logging, "Hello World");

    rh4nvarInitList(&props.bodyvars);

    int index[3] = { 0, -1, -1};
    rh4nvarCreateNewStringArray(&props.bodyvars, NULL, "INFIELD44", 1, length);
    rh4nvarSetStringArrayEntry(&props.bodyvars, NULL, "INFIELD44", index, "1.1");
    index[0] = 1;
    rh4nvarSetStringArrayEntry(&props.bodyvars, NULL, "INFIELD44", index, "1.2");
    index[0] = 2;
    rh4nvarSetStringArrayEntry(&props.bodyvars, NULL, "INFIELD44", index, "1.3");

    rh4nvarCreateNewGroup(&props.bodyvars, "GRP1");
    rh4nvarCreateNewString(&props.bodyvars, "GRP1", "GRPFIELD1", "Hello Group");

    rh4nvarPrintList(&props.bodyvars, &props);


    if((utilsret = rh4nUtilsloadSharedLibrary(NULL, RH4N_SONAME_NATURAL, &shnatlib, errorstr)) != RH4N_RET_OK) { 
        fprintf(stderr, "Loading [%s] failed: [%s]\n", RH4N_SONAME_NATURAL, errorstr); fflush(stderr);
        return(utilsret); 
    }
    if((nnifuncs = rh4nUtilsgetNNIFunctions(NULL, shnatlib, errorstr)) == NULL) {
        fprintf(stderr, "Loading NNI function failed: [%s]\n", errorstr); fflush(stderr);
        rh4nUtilscloseSharedLibrary(shnatlib);
        return(RH4N_RET_NNI_ERR);
    }

    printf("Init natural session with [%s]\n", props.natparms);
    if((nniret = nnifuncs->pf_nni_initialize(nnifuncs, props.natparms, NULL, NULL)) != NNI_RC_OK) {
        printf("natural init failed. NNI ret: %d\n", nniret);
        return(RH4N_RET_INTERNAL_ERR);
    }
    printf("Successfully init natural session\n");

    if((naturalparms = genjsontestParmGen(nnifuncs, &props)) == NULL) {
        printf("Could not init natural parameters. NNI err: [%d]\n", nniret);
        return(RH4N_RET_INTERNAL_ERR);
    }
    printf("Successfully init natural parameters: [%p]\n", nnifuncs);

    printf("Logon into Library [%s]\n", props.natlibrary);
    if((nniret = nnifuncs->pf_nni_logon(nnifuncs, props.natlibrary, NULL, NULL)) != NNI_RC_OK) {
        printf("Could not logon to library [%s]. NNI ret: %d\n", nniret);
        return(RH4N_RET_INTERNAL_ERR);
    }
    printf("Successfully logedon to library\n");

    printf("Calling natural program [%s]\n", props.natprogram);
    if((nniret = nnifuncs->pf_nni_callnat(nnifuncs, props.natprogram, 2, naturalparms, &naturalex)) != NNI_RC_OK) {
        printf("Natural Error\n");
        printf("\tMessageNumber: %d.\n", naturalex.natMessageNumber );
        printf("\tMessageText:   %s\n", naturalex.natMessageText ? naturalex.natMessageText : "0" );
        printf("\tLibrary:       %s.\n", naturalex.natLibrary ? naturalex.natLibrary : "0" );
        printf("\tMember:        %s.\n", naturalex.natMember ? naturalex.natMember : "0" );
        printf("\tName:          %s.\n", naturalex.natName ? naturalex.natName : "0" );
        printf("\tMethod:        %s.\n", naturalex.natMethod ? naturalex.natMethod : "0" );
        printf("\tLine:          %d.\n", naturalex.natLine );
        return(RH4N_RET_INTERNAL_ERR);
    }
    
    rh4nUtilscloseSharedLibrary(shnatlib);
    return(0);
}

struct parameter_description* genjsontestParmGen(pnni_611_functions nnifuncs, RH4nProperties *properties) {
    void *pvnniparms = NULL;
    int nniret = 0;
    static struct parameter_description nniparms[2];

    printf("Start creating natural parameter set\n");

    if((nniret = nnifuncs->pf_nni_create_parm(nnifuncs, 2, &pvnniparms)) != NNI_RC_OK) {
        printf("Could not init natural parms. Ret = %d\n", nniret);
        return(NULL);
    }

    //Set natural parm zero
    if((nniret = nnifuncs->pf_nni_init_parm_d(nnifuncs, 0, pvnniparms, NNI_TYPE_ALPHA, NNI_FLG_PROTECTED)) != NNI_RC_OK) {
        printf("Could not create natural parm 0. Ret = %d\n", nniret);
        return(NULL);
    }

    if((nniret = nnifuncs->pf_nni_put_parm(nnifuncs, 0, pvnniparms, strlen(properties->httpreqtype), properties->httpreqtype)) != NNI_RC_OK) {
        printf("Could not set value for natural parm 0. Ret = %d\n", nniret);
        return(NULL);
    }

    //Set natural parm one
    if((nniret = nnifuncs->pf_nni_init_parm_d(nnifuncs, 1, pvnniparms, NNI_TYPE_BIN, NNI_FLG_PROTECTED)) != NNI_RC_OK) {
        printf("Could not create natural parm 1. Ret = %d\n", nniret);
        return(NULL);
    }

    if((nniret = nnifuncs->pf_nni_put_parm(nnifuncs, 1, pvnniparms, sizeof(RH4nProperties*), &properties)) != NNI_RC_OK) {
        printf("Could not set value for natural parm 1. Ret = %d\n", nniret);
        return(NULL);
    }

    if((nniret = nnifuncs->pf_nni_get_parm_info(nnifuncs, 0, pvnniparms, &nniparms[0])) != NNI_RC_OK) {
        printf("Could not get parameter description of parameter 0. Ret = %d\n", nniret);
        return(NULL);
    }

    if((nniret = nnifuncs->pf_nni_get_parm_info(nnifuncs, 1, pvnniparms, &nniparms[1])) != NNI_RC_OK) {
        printf("Could not get parameter description of parameter 1. Ret = %d\n", nniret);
        return(NULL);
    }
    return(nniparms);
}
