#ifndef RH4NNATCALL
#define RH4NNATCALL

//Private
struct parameter_description* rh4nNaturalParamsGeneration(pnni_611_functions, RH4nProperties*, int*);
void rh4nlogInternalError(RH4nProperties *props, char *error_str);
void rh4nHandleNaturalError(RH4nProperties*, int, struct natural_exception);
void rh4nlogNaturalError(RH4nProperties*, struct natural_exception);
int rh4nCallNatural(RH4nProperties *props);

#define RH4N_TEMPLATE_NAT_JSON "{" \
                               "\"error\":true," \
                               "\"type\":\"natural\"," \
                               "\"naterror\":{"\
                               "\"natMessageNumber\":%d," \
                               "\"natMessageText\":\"%s\"," \
                               "\"natLibrary\":\"%s\","\
                               "\"natMember\":\"%s\"," \
                               "\"natName\":\"%s\"," \
                               "\"natMethod\":\"%s\"," \
                               "\"natLine\":%d" \
                               "}}"

#define RH4N_TEMPLATE_NAT_HTML "<html>" \
                               "<head><title>Natural runtime error: %d</title></head>" \
                               "<body>" \
                               "<h1>A natural runtime error occurred</h1>" \
                               "<p><span>Error-Nr.:%d</span></p>" \
                               "<p><span>Error-Message: %s</span></p>" \
                               "<p><span>Naturallibrary: %s</span></p>" \
                               "<p><span>Naturalprogram: %s</span></p>" \
                               "<p><span>Function/Class name: %s</span></p>" \
                               "<p><span>Method/Property name: %s</span></p>" \
                               "<p><span>Line-Nr.: %d</span></p>" \
                               "</body></html>"

#endif
