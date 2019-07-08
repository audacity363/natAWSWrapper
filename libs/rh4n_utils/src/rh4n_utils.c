#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <dlfcn.h>
#include <errno.h>
#include <ctype.h>

#include "rh4n.h"
#include "rh4n_utils.h"

void RemoveSpaces(char* source)
{
    char* i = source;
    char* j = source;
    while(*j != 0)
    {
        *i = *j++;
        if(*i != ' ')
            i++;
    }
    *i = 0;
}

char *StripTrailingSpaces(char *str)
{
    char *end;

    printf("Strip Spaces:\n");
    printf("Working with [%s]\n", str);

    end = str + strlen(str) - 1;
    while(end > str && isspace(*end)) end--;

    // Write new null terminator
    *(end+1) = 0;

    return str;
}

int str_isdigit(char *str)
{
    int length = strlen(str);
    int i;

    for(i=0; i < length; i++)
    {
        if(isdigit(str[i]) == 0)
            return(-1);
    }
    return(0);
}

int strn_isdigit(char *str, int length)
{
    int i;

    for(i=0; i < length; i++)
    {
        if(isdigit(str[i]) == 0)
            return(-1);
    }
    return(0);
}

void char_strcat(char *str, char character)
{
    char tmp_str[2];
    tmp_str[0] = character;
    tmp_str[1] = '\0';

    strcat(str, tmp_str);
}

void rh4nUtilsTrimSpaces(char *input)
{
    int length = strlen(input);

    for(; length >= -1; length--)
    {
        if(input[length] != 0x20 && input[length] != 0x00)
        {
            input[length+1] = '\0';
            break;
        }
    }
}

void rh4nUtilsTrimSpaces4B(wchar_t *input)
{
    int byte_length = wcslen(input)*sizeof(wchar_t);

    void *start = (void*)input,
         *end = ((void*)input)+byte_length;
    char c_null = 0x00,
         c_space = 0x20;

    while(end != start)
    {
        if(memcmp(end, &c_null, 1) != 0 && memcmp(end, &c_space, 1) != 0)
            break;
        memset(end, 0x00, 1);
        end--;
    }
}

char *str_toUpper(char *input)
{
    static char output[1024];
    int i = 0;

    memset(output, 0x00, sizeof(output));

    if(strlen(input)+1 > sizeof(output))
    {
        return(NULL);
    }

    for(i=0; i < strlen(input); i++)
    {
        output[i] = toupper(input[i]);
    }
    output[i+1] = '\0';

    return(output);
}

char *str_toLower(char *input)
{
    static char output[1024];
    int i = 0;

    memset(output, 0x00, sizeof(output));

    if(strlen(input)+1 > sizeof(output))
    {
        return(NULL);
    }

    for(i=0; i < strlen(input); i++)
    {
        output[i] = tolower(input[i]);
    }
    output[i+1] = '\0';

    return(output);
  
}

int rh4nUtilsloadSharedLibrary(RH4nProperties *props, char *name, void **ppsharedLibrary, char *error_str) {
    void *psharedLibrary = NULL;

    if(props) { rh4n_log_debug(props->logging, "Try to open shared object [%s]", name); }

    if((psharedLibrary = dlopen(name, RTLD_NOW)) == NULL) {
        if(props) { rh4n_log_error(props->logging, "Failed to open library [%s] - %s", name, dlerror()); }
        sprintf(error_str,"Failed to open library [%s] - %s", name, dlerror());
        return(RH4N_RET_INTERNAL_ERR);
    }

    *ppsharedLibrary = psharedLibrary;
    return(RH4N_RET_OK);
}

void rh4nUtilscloseSharedLibrary(void *psharedLibrary) {
    dlclose(psharedLibrary);
}

pnni_611_functions rh4nUtilsgetNNIFunctions(RH4nProperties *props, void *psharedLibrary, char *error_str) {
    pnni_611_functions nnifuncs = NULL;
    PF_NNI_GET_INTERFACE pf_nni_get_interface = NULL;
    int nniret = 0;

    if(props) { rh4n_log_debug(props->logging, "Try to get nni functions"); }

    pf_nni_get_interface = (PF_NNI_GET_INTERFACE)dlsym(psharedLibrary, "nni_get_interface");
    if(pf_nni_get_interface == NULL) {
        if(props) { rh4n_log_error(props->logging, "Could not find the symbol \"nni_get_interface\" in given library"); }
        sprintf(error_str, "Could not find the symbol \"nni_get_interface\" in given library");
        return(NULL);
    }

    if((nniret = (pf_nni_get_interface)(NNI_VERSION_CURR, (void**)&nnifuncs)) != NNI_RC_OK) {
        if(props) { rh4n_log_error(props->logging, "Loading nni function struct failed. Ret = %d", nniret); }
        sprintf(error_str, "Loading nni function struct failed. Ret = %d", nniret);
        return(NULL);
    }

    return(nnifuncs);
}

void convert1Bto4BString(char *inbuffer, wchar_t *outbuffer, int length)
{
    int i, offset;

    void *v_in, *v_out, *v_tmp;

    v_in = (void*)inbuffer;
    v_out = (void*)outbuffer;

    for(i=0; i < length; i++) {
        offset = (sizeof(wchar_t)*i)+3;
        v_tmp = v_out+offset;
        memcpy(v_tmp, v_in+i, 1);
    }
}
