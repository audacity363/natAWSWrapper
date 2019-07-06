#ifndef RH4NLDAPARSER
#define RH4NLDAPARSER

#ifdef __cplusplus
extern "C" {
#endif

enum
{
    RH4NLDA_SAG_COMMENT,
    RH4NLDA_SAG_DEFINE, 
    RH4NLDA_SAG_DEFINE_CONST,
    RH4NLDA_SAG_GROUP_MEMBER,
    RH4NLDA_SAG_GROUP_HEAD,
    RH4NLDA_SAG_REDEFINE,
    RH4NLDA_SAG_REDEFINED_VALUES,
    RH4NLDA_SAG_INIT,
    RH4NLDA_SAG_PRE_INIT,
    RH4NLDA_SAG_DYNAMIC_DF
};

typedef struct RH4NLDAEntry_s {
    int level;
    char type;
    char name[NNI_LEN_NAME+1];
    int dimensions;
    int length;
    int arraylength[3];

    struct RH4NLDAEntry_s *prev;
    struct RH4NLDAEntry_s *next;
    struct RH4NLDAEntry_s *nextlvl;
} RH4nLDAEntry_t;

//Public
int rh4nldaStartParser(char*, RH4nLDAEntry_t**, RH4nProperties*, char*);
void rh4nldaPrintList(RH4nLDAEntry_t*, RH4nProperties*);
void rh4nldaFreeList(RH4nLDAEntry_t*);

//Private
int rh4nldaReadLine(FILE*, char**, RH4nProperties*);
int rh4nldaParseLine(char*, RH4nLDAEntry_t**, RH4nProperties*, char*);
int rh4nldaSaveEntry(RH4nLDAEntry_t*, RH4nProperties*); 
void rh4nldaInitEntry(RH4nLDAEntry_t*);
int rh4nldaCheckVartype(int);
int rh4nldaGetArrayType(char*, int*, int*, RH4nProperties*, char*);
char *rh4nldastrtok(char*, char);
int rh4nldaGetVariablenLength(char**, int*, int, char*);
void rh4nldaPrintFork(RH4nLDAEntry_t*, RH4nProperties*, int);
void rh4nldaFreeFork(RH4nLDAEntry_t*);
int rh4nldaGetVarname(char **line, char *varname);

#define RH4N_LDA_HEADERSIZE 72
#define RH4N_LDA_VARTYPE_UNSUPPORTED -1
#define RH4N_LDA_BREAK -2 

#define RH4NLDA_DEFINE_DATA "DEFINE DATA LOCAL "
#define RH4NLDA_END_DEFINED_DATA "END-DEFINE "
#define RH4NLDA_REDEFINE "REDEFINE"
#define RH4NLDA_VIEW "VIEW OF"
#define RH4NLDA_CONST "CONST"
#define RH4NLDA_INIT "INIT"
#define RH4NLDA_HEADER_OFFSET 72
#define RH4NLDA_MAX_LEN_CHAR 6

#define RH4NLDA_COMPARE_HEAD_STR(x) strcmp(x, RH4NLDA_DEFINE_DATA) == 0
#define RH4NLDA_COMPARE_FOOT_STR(x) strcmp(x, RH4NLDA_END_DEFINED_DATA) == 0
#define RH4NLDA_COMPARE_REDEFINE_STR(x) strncmp(x, RH4NLDA_REDEFINE, sizeof(RH4NLDA_REDEFINE)) == 0
#define RH4NLDA_CHECK_IF_VIEW(x) strstr(x, RH4NLDA_VIEW) != NULL
#define RH4NLDA_CHECK_IF_CONST(x) strstr(x, RH4NLDA_CONST) != NULL
#define RH4NLDA_CHECK_IF_INIT(x) strstr(x, RH4NLDA_INIT) != NULL

extern RH4nLDAEntry_t *g_rh4nldaentries, *g_rh4nldacurptr;

#ifdef __cplusplus
}
#endif

#endif
