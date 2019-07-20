#ifndef RH4NLIBUTILS
#define RH4NLIBUTILS

#ifdef __cplusplus
extern "C" {
#endif

void rh4nUtilsTrimSpaces(char *input);
void rh4nUtilsTrimSpaces4B(wchar_t *input);
char *str_toLower(char *input);
char *str_toUpper(char *input);
void rh4nUtilscloseSharedLibrary(void *psharedLibrary);
void convert1Bto4BString(char*, wchar_t*, int);
int rh4nUtilsDumpProperties(char*, RH4nProperties*);
int rh4nUtilsLoadProperties(char*, RH4nProperties*);

#define RH4N_UTILS_READ(file, buff, len, ret) if((ret = read(file, buff, len) != len)) return(RH4N_RET_FILE_ERR)

#ifdef RH4N_STD
int rh4nUtilsloadSharedLibrary(RH4nProperties*, char*, void**, char*);
pnni_611_functions rh4nUtilsgetNNIFunctions(RH4nProperties*, void*, char*);
#endif

#ifdef __cplusplus
}
#endif

#endif
