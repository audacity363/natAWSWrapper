#ifndef RH4NNATUSER_VARHANDLING
#define RH4NNATUSER_VARHANDLING

#define MAXI1 0xFF
#define MAXI2 0xFFFF
#define MAXI4 0xFFFFFFFF

#define I1SIZE 1
#define I2SIZE 2
#define I4SIZE 4
#define LSIZE 1
#define F4SIZE 4
#define F8SIZE 8
#define GET_VAR_RET_PARM_POS 2
#define MAX_NAT_N_SIZE 29

typedef struct {
    void *parmhandle;
    int parmindex;
    struct parameter_description *desc;
    char *tmpname;
    pnni_611_functions nnifuncs;
    RH4nVarList *varlist;
    RH4nProperties *props;
    char *errorstr;
} RH4nNatVarHandleParms;

struct RH4nNatVarHandling {
    char vartype;
    int (*callback)(RH4nNatVarHandleParms*);
};

//Var handling
//String
int rh4nnatStringHandler(RH4nNatVarHandleParms*);
int rh4nnatStringArrayHandler(RH4nNatVarHandleParms*);
int rh4nnatSaveStringArrayEntry(RH4nNatVarHandleParms*,int*);

int rh4nnatputString(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputStringArray(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputStringArrayEntry(RH4nNatVarHandleParms*, char*, char*, int*);
//Integer
int rh4nnatIntegerHandler(RH4nNatVarHandleParms*);
int rh4nnatIntegerArrayHandler(RH4nNatVarHandleParms*);
int rh4nnatSaveIntegerArrayEntry(RH4nNatVarHandleParms*,int*);

int rh4nnatputInteger(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputIntegerArray(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputIntegerArrayEntry(RH4nNatVarHandleParms*, char*, char*, int*);
//Boolean
int rh4nnatBoolHandler(RH4nNatVarHandleParms*);
int rh4nnatBoolArrayHandler(RH4nNatVarHandleParms*);
int rh4nnatSaveBoolArrayEntry(RH4nNatVarHandleParms*,int*);

int rh4nnatputBool(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputBoolArray(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputBoolArrayEntry(RH4nNatVarHandleParms*, char*, char*, int*);
//Float
int rh4nnatFloatHandler(RH4nNatVarHandleParms*);
int rh4nnatFloatArrayHandler(RH4nNatVarHandleParms*);
int rh4nnatSaveFloatArrayEntry(RH4nNatVarHandleParms*,int*);
double rh4nnatConvertFloat(float);

int rh4nnatputFloat(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputFloatArray(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputFloatArrayEntry(RH4nNatVarHandleParms*, char*, char*, int*);
//DateTime
int rh4nnatDateTimeHandler(RH4nNatVarHandleParms*);
int rh4nnatDateTimeArrayHandler(RH4nNatVarHandleParms*);
int rh4nnatSaveDateTimeArrayEntry(RH4nNatVarHandleParms*,int*);
//Numeric/Packed
int rh4nnatNumericPackedHandler(RH4nNatVarHandleParms*);
int rh4nnatNumericPackedArrayHandler(RH4nNatVarHandleParms*);
int rh4nnatSaveNumericPackedArrayEntry(RH4nNatVarHandleParms*,int*);


int rh4nnatputAtoN(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputAtoNArray(RH4nNatVarHandleParms*, char*, char*);
int rh4nnatputAtoNArrayEntry(RH4nNatVarHandleParms*, char*, char*, int*);
//UString
int rh4nnatUnicodeHandler(RH4nNatVarHandleParms*);
int rh4nnatUnicodeArrayHandler(RH4nNatVarHandleParms*);
int rh4nnatSaveUnicodeArrayEntry(RH4nNatVarHandleParms*,int*);
void rh4nnatConvert2BTo4BString(char*, wchar_t*, int);
#endif
