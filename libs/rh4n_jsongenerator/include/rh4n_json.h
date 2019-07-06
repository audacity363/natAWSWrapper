#ifndef RH4NJSONGENERATOR
#define RH4NJSONGENERATOR

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int aimdim;
    int length[3];
    int index[3];
} RH4nJSONObjectArrayParms_t;

//Public
void rh4njsonPrintJSON(RH4nVarList*, FILE*, RH4nProperties*);
int rh4njsonPrintJSONToFile(RH4nVarList*, char*, RH4nProperties*);


//Private
bool rh4njsonCheckIfJSONArray(RH4nVarEntry_t*, RH4nProperties*, short);
int rh4njsonCountGroups(RH4nVarEntry_t*, RH4nProperties*);
bool rh4njsonCheckIfOnlyOneGroup(RH4nVarEntry_t*, RH4nProperties*);
void rh4njsonPrintObject(RH4nVarEntry_t*, FILE*, int, RH4nProperties*);
void rh4njsonPrintJSONArray(RH4nVarObj*, FILE*, RH4nProperties*);
void rh4njsonPrintJSONObjectArray(RH4nVarEntry_t *target, FILE *outputfile, RH4nProperties *props);
void rh4njsonpregetCommonDimension(RH4nVarEntry_t *target, int length[3], int maxdim[3], RH4nProperties *props);
void rh4njsoncheckCommonDimension(int curlength[3], int length[3], int dimOK[3]);
void rh4njsonMergeCommonDimResults(int[3], int[3], int[3], int[3], RH4nProperties*);
int rh4njsongetCommonDimension(int[3]); 
int rh4njsonPrintObjectArrayEntry(RH4nVarEntry_t*, FILE*, int, RH4nJSONObjectArrayParms_t, RH4nProperties*);

#ifdef __cplusplus
}
#endif

#endif
