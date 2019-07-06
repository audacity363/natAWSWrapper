#ifndef RH4NVAR2NAME
#define RH4NVAR2NAME

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char groupname[NNI_LEN_NAME];
    char varname[NNI_LEN_NAME];
    int position;
} RH4nVarPosition_t;

typedef struct {
    int length;
    RH4nVarPosition_t *parm_positions;
} RH4nVarPositions_t;

typedef struct {
    int length;
    char **groups;
} RH4nParentGroups_t;

//defined in rh4n_v2n_resolvenames.c
extern RH4nParentGroups_t parents;

#ifdef RH4NLDAPARSER
int rh4nv2nStart(RH4nVarList*, RH4nLDAEntry_t*, char*, RH4nProperties*, char*);
int rh4nv2nStartPosition(RH4nLDAEntry_t*, char*, RH4nVarPositions_t*, RH4nProperties*, char*);
int rh4nv2ncmpVarNumbers(RH4nVarList*, RH4nLDAEntry_t*, RH4nProperties*, char*);
int rh4nv2ncountVarsinLDAGroup(RH4nLDAEntry_t*);
int rh4nv2nSearchTargetStruct(RH4nLDAEntry_t*, char*, RH4nLDAEntry_t**, char*);
int rh4nv2nSavePosition(char*, char*, int, RH4nVarPositions_t*, RH4nProperties*);
int rh4nv2nMatchPosition(RH4nLDAEntry_t *, char*, int*, RH4nVarPositions_t*, RH4nProperties*); 
RH4nVarEntry_t *rh4nv2nMatchNames(RH4nVarEntry_t*, RH4nLDAEntry_t*, RH4nVarList*, char*, RH4nProperties*, int*);
#endif

#ifdef __cplusplus
}
#endif

#endif
